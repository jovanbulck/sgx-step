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

#include <sgx_urts.h>
#include <signal.h>
#include <sys/mman.h>

#include "Enclave/encl_u.h"
#include "libsgxstep/debug.h"
#include "libsgxstep/elf_parser.h"
#include "libsgxstep/enclave.h"
#include "libsgxstep/pt.h"

#define DBG_ENCL 1

void *data_pt = NULL, *data_page = NULL, *code_pt = NULL;
int fault_fired = 0, aep_fired = 0;
sgx_enclave_id_t eid = 0;

void aep_cb_func(void) {
    gprsgx_region_t gprsgx = {0};
    uint64_t erip = edbgrd_erip() - (uint64_t)get_enclave_base();
    info("Hello world from AEP callback with erip=%#llx! Resuming enclave..",
         erip);

    edbgrd(get_enclave_ssa_gprsgx_adrs(), &gprsgx, sizeof(gprsgx_region_t));
    dump_gprsgx_region(&gprsgx);

    aep_fired++;
}

void fault_handler(int signo, siginfo_t *si, void *ctx) {
    ASSERT(fault_fired < 5);

    switch (signo) {
        case SIGSEGV:
            info("Caught page fault (base address=%p)", si->si_addr);
            break;

        default:
            info("Caught unknown signal '%d'", signo);
            abort();
    }

    if (si->si_addr == data_page) {
        info("Restoring data access rights..");
        ASSERT(!mprotect(data_page, 4096, PROT_READ | PROT_WRITE));
        print_pte_adrs(data_pt);
    } else if (si->si_addr == code_pt) {
        info("Restoring code access rights..");
        ASSERT(!mprotect(code_pt, 4096, PROT_READ | PROT_EXEC));
        print_pte_adrs(code_pt);
    } else {
        info("Unknown #PF address!");
    }

    fault_fired++;
}

void attacker_config_page_table(void) {
    struct sigaction act, old_act;

    /* NOTE: finer-grained permissions can be revoked using
     * `remap_page_table_level` and directly editing PTEs (e.g., app/memcmp),
     * but care needs to be taken as the Linux kernel expects PTE inversion
     * when unmapping PTEs (only relevant for MARK_NOT_PRESENT). We simply use
     * mprotect here as we don't need fine-grained permissions or performance
     * for this example, and mprotect transparently takes care of PTE
     * inversion.
     */
    info("revoking data page access rights..");
    data_pt = get_symbol_offset("array") + get_enclave_base();
    data_page = (void *)((uintptr_t)data_pt & ~PFN_MASK);
    info("data at %p with PTE:", data_pt);
    print_pte_adrs(data_pt);
    ASSERT(!mprotect(data_page, 4096, PROT_NONE));
    print_pte_adrs(data_pt);

    info("revoking code page access rights..");
    code_pt = get_symbol_offset("page_aligned_func") + get_enclave_base();
    info("code at %p with PTE:", code_pt);
    print_pte_adrs(code_pt);
    ASSERT(!mprotect(code_pt, 4096, PROT_NONE));
    print_pte_adrs(code_pt);

    /* Specify #PF handler with signinfo arguments */
    memset(&act, 0, sizeof(sigaction));
    act.sa_sigaction = fault_handler;
    act.sa_flags = SA_RESTART | SA_SIGINFO;

    /* Block all signals while the signal is being handled */
    sigfillset(&act.sa_mask);
    ASSERT(!sigaction(SIGSEGV, &act, &old_act));
}

int main(int argc, char **argv) {
    sgx_launch_token_t token = {0};
    int retval = 0, updated = 0;
    char old = 0x00, new = 0xbb;

    info("Creating enclave...");
    SGX_ASSERT(sgx_create_enclave("./Enclave/encl.so", /*debug=*/DBG_ENCL,
                                  &token, &updated, &eid, NULL));

    info("Dry run to allocate pages");
    SGX_ASSERT(enclave_dummy_call(eid, &retval));
    SGX_ASSERT(page_aligned_func(eid));

    register_symbols("./Enclave/encl.so");
    attacker_config_page_table();
    register_aep_cb(aep_cb_func);
    print_enclave_info();
    dump_enclave_exec_pages();

    info_event("reading/writing debug enclave memory..");
    edbgrd(data_pt, &old, 1);
    edbgwr(data_pt, &new, 1);
    edbgrd(data_pt, &new, 1);
    info("data at %p (page %p): old=0x%x; new=0x%x", data_pt, data_page,
         old & 0xff, new & 0xff);

    info_event("calling enclave data page fault..");
    SGX_ASSERT(enclave_dummy_call(eid, &retval));

    info_event("calling enclave code page fault..");
    SGX_ASSERT(page_aligned_func(eid));

    info("all is well; exiting..");
    ASSERT(fault_fired && aep_fired);
    SGX_ASSERT(sgx_destroy_enclave(eid));
    return 0;
}
