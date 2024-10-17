/*
 *  This file is part of the SGX-Step enclave execution control framework.
 *
 *  Copyright (C) 2017 Jo Van Bulck <jo.vanbulck@cs.kuleuven.be>,
 *                     Raoul Strackx <raoul.strackx@cs.kuleuven.be>
 *
 *  SGX-Step is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SGX-Step is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SGX-Step. If not, see <http://www.gnu.org/licenses/>.
 */
#if !NO_SGX

#include "enclave.h"
#include "debug.h"
#include <sys/ioctl.h>
#include <inttypes.h>
#include "../kernel/sgxstep_ioctl.h"
#include "pt.h"
#include "cache.h"
#include <fcntl.h>
#include <string.h>

/* Custom AEP get/set functions from patched SGX SDK urts. */
void* sgx_get_aep(void);
void sgx_set_aep(void* aep);
void* sgx_get_tcs(void);

/* See aep_trampoline.S to see how these are used. */
extern void sgx_step_aep_trampoline(void);
aep_cb_t sgx_step_aep_cb = NULL;
uint64_t nemesis_tsc_eresume = 0x0;
int sgx_step_eresume_cnt = 0;
int sgx_step_do_trap = 0;

extern int fd_step;
struct sgx_step_enclave_info victim = {0};
int ioctl_init = 0;
int fd_self_mem = -1;

void register_aep_cb(aep_cb_t cb)
{
    sgx_set_aep(sgx_step_aep_trampoline);
    sgx_step_aep_cb = cb;
}

void register_enclave_info(void)
{
    FILE *fd_self_maps;
    uint64_t start, end, prev_end = 0;
    char *pathname = NULL;
    char exec;
    int is_enclave = 0, prev_is_enclave = 0, is_isgx, is_kern;
    memset(&victim, 0x0, sizeof(victim));

    /* Parse /proc/self/maps to detect any enclaves mapped in the address space.
     * Expected format: "start-end perms offset dev inode optional_pathname"
     * For documentation of /proc/pid/maps, see `man 5 proc`.
     *
     * NOTES: - victim.tcs is set by the patched untrusted runtime on first
     *          enclave entry (e.g., as part of sgx_create_enclave)
     *        - enclave mappings are expected to be backed by a recognized SGX
     *          driver (i.e., pathname /dev/isgx or /dev/sgx_enclave)
     *        - only supports a single enclave that is expected to be
     *          contiguously mapped in the address space
     */
    #if LIBSGXSTEP_DEBUG
        debug("cat /proc/self/maps");
        char command[256];
        sprintf(command, "cat /proc/%d/maps", getpid());
        system(command);
        debug("------");
    #endif
    ASSERT((fd_self_maps = fopen("/proc/self/maps", "r")) >= 0);
    while (fscanf(fd_self_maps, "%lx-%lx %*c%*c%c%*c %*x %*x:%*x %*[0-9 ]%m[^\n]",
                  &start, &end, &exec, &pathname) > 0)
    {
        debug("%p - %p %c %s", (void*) start, (void*) end, exec, pathname);
        is_isgx = (pathname != NULL) && strstr(pathname, "/dev/isgx") != NULL;
        is_kern = (pathname != NULL) && strstr(pathname, "/dev/sgx_enclave") != NULL;
        is_enclave = is_isgx || is_kern;

        if (is_enclave)
        {
            if (!victim.drv)
            {
                debug("Found %s enclave at %p in /proc/self/maps", pathname, (void*) start);

                victim.base = (uint64_t) start;
                victim.drv = is_isgx ? "/dev/isgx" : "/dev/sgx_enclave";
            }
            if (exec == 'x')
            {
                debug("Found enclave executable range [%p,%p]", (void*) start, (void*) end);
                WARN_ON(victim.exec_limit != 0, "enclave contains >1 executable range");
                victim.exec_base = start;
                victim.exec_limit = end;
            }
        }
        else if (prev_is_enclave && !is_enclave)
        {
            victim.limit = (uint64_t) prev_end;
        }

        if (pathname != NULL)
        {
            free(pathname);
            pathname = NULL;
        }

        prev_is_enclave = is_enclave;
        prev_end = end;
    }
    ASSERT( victim.drv && "no enclave found in /proc/self/maps");

    victim.tcs = (uint64_t) sgx_get_tcs();
    victim.aep = (uint64_t) sgx_get_aep();
    info("tcs at %lx; aep at %lx", victim.tcs, victim.aep);
    ASSERT( victim.tcs >= victim.base && victim.tcs < victim.limit);
    ioctl_init = 1;
}

void *get_enclave_base(void)
{
    if (!ioctl_init) register_enclave_info();

    return (void*)((uintptr_t) victim.base);
}

void *get_enclave_limit(void)
{
    if (!ioctl_init) register_enclave_info();

    return (void*)((uintptr_t) victim.limit);
}

char *get_enclave_drv(void)
{
    if (!ioctl_init) register_enclave_info();

    return victim.drv;
}

int get_enclave_size(void)
{
    if (!ioctl_init) register_enclave_info();

    return (int) (victim.limit - victim.base);
}

int get_enclave_exec_range(uint64_t *start, uint64_t *end)
{
    if (!ioctl_init) register_enclave_info();
    ASSERT( victim.exec_base && victim.exec_limit);

    if (start) *start = victim.exec_base;
    if (end) *end = victim.exec_limit;
    return (victim.exec_limit - victim.exec_base) / PAGE_SIZE_4KiB;
}

/* allocated once and freed on process exit */
uint64_t **enclave_exec_ptes = NULL;
size_t enclave_exec_ptes_len = 0;

#define ENCLAVE_EXEC_NB2ADDR(nb) ((void*) (victim.exec_base + nb*PAGE_SIZE_4KiB))

static void alloc_enclave_exec_ptes(void)
{
    int i, sz;
    uint64_t start, end;
    uint64_t *pte;

    ASSERT( !enclave_exec_ptes);
    sz = get_enclave_exec_range(&start, &end);
    enclave_exec_ptes_len = sz;
    enclave_exec_ptes = malloc(sz * sizeof(uint64_t*));
    ASSERT( enclave_exec_ptes);

    for (i = 0; i < enclave_exec_ptes_len; i++)
    {
        pte = remap_page_table_level(ENCLAVE_EXEC_NB2ADDR(i), PTE);
        ASSERT(pte);
        enclave_exec_ptes[i] = pte;
    }
}

void mark_enclave_exec_not_accessed(void)
{
    if (!enclave_exec_ptes)
        alloc_enclave_exec_ptes();

    for (int i = 0; i < enclave_exec_ptes_len; i++)
    {
        /*
         * NOTE: clearing the PTE "accessed" bit forces the CPU to take a
         * ucode-assisted page-table walk for the first instruction following
         * ERESUME, which causes that instruction to be much longer. We
         * additionally flush the PTEs from the cache to further delay the
         * page-table walk and increase the landing space for the timer interrupt.
         */
        if (PRESENT(*enclave_exec_ptes[i]))
        {
            *enclave_exec_ptes[i] = MARK_NOT_ACCESSED(*enclave_exec_ptes[i]);
            flush(enclave_exec_ptes[i]);
        }
    }
}

uint64_t is_enclave_exec_accessed(void)
{
    ASSERT (enclave_exec_ptes && "first call mark_enclave_exec_not_accessed");

    for (int i = 0; i < enclave_exec_ptes_len; i++)
    {
        if (PRESENT(*enclave_exec_ptes[i]) && ACCESSED(*enclave_exec_ptes[i]))
            return (uint64_t) ENCLAVE_EXEC_NB2ADDR(i);
    }
    return 0;
}

void dump_enclave_exec_pages(void)
{
    if (!enclave_exec_ptes)
        alloc_enclave_exec_ptes();

    for (int i = 0; i < enclave_exec_ptes_len; i++)
    {
        info("%09lx: P=%ld; A=%ld", ENCLAVE_EXEC_NB2ADDR(i) - get_enclave_base(),
                PRESENT(*enclave_exec_ptes[i]), ACCESSED(*enclave_exec_ptes[i]));
    }
}
/*
 * NOTE: we simply read from the standard Linux interface /proc/self/mem, which
 * will call the associated SGX driver (i.e., /dev/isgx or /dev/sgx_enclave) to
 * handle the memory read/write request. This is easier than calling the
 * privileged EDBGRD/EDBGWR instructions directly ourselves, as this way we
 * don't have to worry about illegal ptrs or #PFs etc.
 *
 * Returns the number of bytes successfully read/written, or a value <0 for
 * production enclaves.
 */
int edbgrdwr(void *adrs, void* res, int len, int write)
{
    int rv;
    if (fd_self_mem < 0)
    {
        ASSERT((fd_self_mem = open("/proc/self/mem", O_RDWR)) >= 0);
    }

    if (!write)
        rv = pread(fd_self_mem, res, len, (off_t) adrs);
    else
        rv = pwrite(fd_self_mem, res, len, (off_t) adrs);

    debug("edbg%s at %p; len=%d; rv=%d", write ? "wr" : "rd", adrs, len, rv);
    #if LIBSGXSTEP_DEBUG
        printf("\tbuf = ");
        dump_hex(res, rv);
    #endif

    //ASSERT(rv >= 0);
    return rv;
}

uint64_t edbgrd_ssa_gprsgx(int gprsgx_field_offset)
{
    uint64_t ret;
    void *ssa_field_addr = get_enclave_ssa_gprsgx_adrs() + gprsgx_field_offset;
    edbgrd(ssa_field_addr, &ret, 8);

    return ret;
}

void* get_enclave_ssa_gprsgx_adrs(void)
{
    uint64_t ossa = 0x0;
    uint32_t cssa = 0x0;
    void *tcs_addr = sgx_get_tcs();
    edbgrd(tcs_addr + SGX_TCS_OSSA_OFFSET, &ossa, sizeof(ossa));
    edbgrd(tcs_addr + SGX_TCS_CSSA_OFFSET, &cssa, sizeof(cssa));

    return get_enclave_base() + ossa + (cssa * SGX_SSAFRAMESIZE) - SGX_GPRSGX_SIZE;
}

void set_debug_optin(void) 
{
    void *tcs_addr = sgx_get_tcs();
    uint64_t flags;
    edbgrd(tcs_addr + SGX_TCS_FLAGS_OFFSET, &flags, sizeof(flags));
    flags |= SGX_FLAGS_DBGOPTIN;
    edbgwr(tcs_addr + SGX_TCS_FLAGS_OFFSET, &flags, sizeof(flags));
}

void print_enclave_info(void)
{
    uint64_t read = 0xff;

    printf( "==== Victim Enclave ====\n" );
    printf( "    Driver: %s\n", get_enclave_drv());
    printf( "    Base:   %p\n", get_enclave_base() );
    printf( "    Limit:  %p\n", get_enclave_limit());
    printf( "    Size:   %d\n", get_enclave_size() );
    printf( "    Exec:   %d pages\n", get_enclave_exec_range(NULL,NULL));
    printf( "    TCS:    %p\n", sgx_get_tcs() );
    printf( "    SSA:    %p\n", get_enclave_ssa_gprsgx_adrs() );
    printf( "    AEP:    %p\n", sgx_get_aep() );

    /* First 8 bytes of TCS must be zero */
    int rv = edbgrd( sgx_get_tcs(), &read, 8);
    printf( "    EDBGRD: %s\n", rv < 0 ? "production" : "debug");
}

void dump_gprsgx_region(gprsgx_region_t *gprsgx_region)
{
    printf("=== SSA/GPRSGX region after AEX ===\n");
    printf("    RAX:      0x%" PRIx64 "\n", gprsgx_region->fields.rax);
    printf("    RCX:      0x%" PRIx64 "\n", gprsgx_region->fields.rcx);
    printf("    RDX:      0x%" PRIx64 "\n", gprsgx_region->fields.rdx);
    printf("    RBX:      0x%" PRIx64 "\n", gprsgx_region->fields.rbx);
    printf("    RSP:      0x%" PRIx64 "\n", gprsgx_region->fields.rsp);
    printf("    RBP:      0x%" PRIx64 "\n", gprsgx_region->fields.rbp);
    printf("    RSI:      0x%" PRIx64 "\n", gprsgx_region->fields.rsi);
    printf("    RDI:      0x%" PRIx64 "\n", gprsgx_region->fields.rdi);
    printf("    R8:       0x%" PRIx64 "\n", gprsgx_region->fields.r8);
    printf("    R9:       0x%" PRIx64 "\n", gprsgx_region->fields.r9);
    printf("    R10:      0x%" PRIx64 "\n", gprsgx_region->fields.r10);
    printf("    R11:      0x%" PRIx64 "\n", gprsgx_region->fields.r11);
    printf("    R12:      0x%" PRIx64 "\n", gprsgx_region->fields.r12);
    printf("    R13:      0x%" PRIx64 "\n", gprsgx_region->fields.r13);
    printf("    R14:      0x%" PRIx64 "\n", gprsgx_region->fields.r14);
    printf("    R15:      0x%" PRIx64 "\n", gprsgx_region->fields.r15);
    printf("    RFLAGS:   0x%" PRIx64 "\n", gprsgx_region->fields.rflags);
    printf("    RIP:      0x%" PRIx64 "\n", gprsgx_region->fields.rip);
    printf("    URSP:     0x%" PRIx64 "\n", gprsgx_region->fields.ursp);
    printf("    URBP:     0x%" PRIx64 "\n", gprsgx_region->fields.urbp);
    printf("    EXITINFO: 0x%" PRIx32 "\n", gprsgx_region->fields.exitinfo);
    printf("    FSBASE:   0x%" PRIx64 "\n", gprsgx_region->fields.fsbase);
    printf("    GSBASE:   0x%" PRIx64 "\n", gprsgx_region->fields.gsbase);
}

#endif
