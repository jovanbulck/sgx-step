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

/* Includes custom AEP get/set functions from patched SGX SDK urts. */
#include <sgx_urts.h>

/* See aep_trampoline.S to see how these are used. */
extern void sgx_step_aep_trampoline(void);
aep_cb_t sgx_step_aep_cb = NULL;
uint64_t sgx_step_tcs    = 0x0;
uint32_t nemesis_tsc_eresume = 0x0, nemesis_tsc_aex = 0x0;
int sgx_step_eresume_cnt = 0;

extern int fd_step;
struct sgx_step_enclave_info victim = {0};
int ioctl_init = 0;

void register_aep_cb(aep_cb_t cb)
{
    sgx_set_aep(sgx_step_aep_trampoline);
    sgx_step_aep_cb = cb;
}

void register_enclave_info(void)
{
    victim.tcs = (uint64_t) sgx_get_tcs();
    victim.aep = (uint64_t) sgx_get_aep();

    step_open();
    ASSERT(ioctl(fd_step, SGX_STEP_IOCTL_VICTIM_INFO, &victim) >= 0);
    ioctl_init = 1;
}

void *get_enclave_base(void)
{
    if (!ioctl_init) register_enclave_info();

    return (void*)((uintptr_t) victim.base);
}

int get_enclave_size(void)
{
    if (!ioctl_init) register_enclave_info();

    return (int) victim.size;
}

void edbgrd(void *adrs, void* res, int len)
{
    edbgrd_t edbgrd_data = {
        .adrs = (uintptr_t) adrs,
        .val = (uintptr_t) res,
        .len = (int64_t) len
    };

    step_open();
    ASSERT( ioctl(fd_step, SGX_STEP_IOCTL_EDBGRD, &edbgrd_data) >= 0 );
}

uint64_t edbgrd_ssa(int ssa_field_offset)
{
    /* NOTE: we cache ossa here to avoid 2 EDBGRD IOCTL calls every time.. */
    static uint64_t ossa = 0x0;
    uint64_t ret;
    void *ssa_field_addr, *tcs_addr = sgx_get_tcs();

    if (!ossa)
    {
        edbgrd(tcs_addr + SGX_TCS_OSSA_OFFSET, &ossa, 8);
    }
    ssa_field_addr = get_enclave_base() + ossa + SGX_SSAFRAMESIZE
                     - SGX_GPRSGX_SIZE + ssa_field_offset;
    edbgrd(ssa_field_addr, &ret, 8);

    return ret;
}

void* get_enclave_ssa_gprsgx_adrs(void)
{
    /* NOTE: we cache ossa here to avoid 2 EDBGRD IOCTL calls every time.. */
    static uint64_t ossa = 0x0;
    void *tcs_addr = sgx_get_tcs();
    if (!ossa)
    {
        edbgrd(tcs_addr + SGX_TCS_OSSA_OFFSET, &ossa, 8);
    }

    return get_enclave_base() + ossa + SGX_SSAFRAMESIZE - SGX_GPRSGX_SIZE;
}

void print_enclave_info(void)
{
    uint64_t read = 0xff;

	printf( "==== Victim Enclave ====\n" );
	printf( "    Base:   %p\n", get_enclave_base() );
	printf( "    Size:   %d\n", get_enclave_size() );
    printf( "    Limit:  %p\n", get_enclave_base()+get_enclave_size() );
	printf( "    TCS:    %p\n", sgx_get_tcs() );
    printf( "    SSA:    %p\n", get_enclave_ssa_gprsgx_adrs() );
	printf( "    AEP:    %p\n", sgx_get_aep() );

    /* First 8 bytes of TCS must be zero */
    edbgrd( sgx_get_tcs(), &read, 8);
    printf( "    EDBGRD: %s\n", read ? "production" : "debug");
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
