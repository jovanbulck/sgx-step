
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
#include <sys/mman.h>

#include "libsgxstep/sgx_tracer.h" // <-------- Include the tracer

#define DEBUG              0
#define DBG_ENCL           1

sgx_enclave_id_t eid = 0;
int irq_cnt = 0;
uint64_t *pte_encl= NULL, *pmd_encl = NULL;
sgx_tracer_t tracer;
void *code_adrs;

/* ================== ATTACKER IRQ/FAULT HANDLERS ================= */
/* Called upon SIGSEGV caused by untrusted page tables. */
void fault_handler(int signo, siginfo_t * si, void  *ctx)
{
    ucontext_t *uc = (ucontext_t *) ctx;

    switch ( signo )
    {
      case SIGSEGV:
        ASSERT( !mprotect( (void*) (((uint64_t) code_adrs) & ~PFN_MASK), 0x1000, PROT_READ | PROT_EXEC ));
        #if DEBUG
            info("Caught page fault (base address=%p)", si->si_addr);
        #endif
	break;
    
      default:
        info("Caught unknown signal '%d'", signo);
        abort();
    }
}

/* Called before resuming the enclave after an Asynchronous Enclave eXit. */
void aep_cb_func(void)
{
    #if DEBUG
        uint64_t erip = edbgrd_erip() - (uint64_t) get_enclave_base();
        info("^^ enclave RIP=%#llx; ACCESSED=%d", erip, ACCESSED(*pte_encl));
    #endif
    irq_cnt++;

    if (irq_cnt > NUM_RUNS*10000)
    {
        info("excessive interrupt rate detected (try adjusting timer interval " \
             "to avoid getting stuck in zero-stepping); aborting...");
        exit(1);
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
    if (ACCESSED(*pte_encl)) // change this such that it is any enclave page is accessed
    {
        sgx_tracer_step(&tracer);
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
    *pmd_encl = MARK_NOT_ACCESSED( *pmd_encl );
    flush(pmd_encl);
    apic_timer_irq( SGX_STEP_TIMER_INTERVAL );
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

    register_enclave_info();
    register_signal_handler( SIGSEGV );
    print_enclave_info();
}

void attacker_config_page_table(void)
{
    code_adrs = get_symbol_offset("dummy_enclave_fn") + get_enclave_base();
    ASSERT( pte_encl    = remap_page_table_level( code_adrs, PTE) );
    ASSERT( PRESENT(*pte_encl) );
    *pte_encl = MARK_NOT_ACCESSED(*pte_encl);
    info("enclave code at %p with PTE", code_adrs);

    print_pte_adrs( code_adrs );
    ASSERT( pmd_encl = remap_page_table_level( get_enclave_base(), PMD) );
    ASSERT( PRESENT(*pmd_encl) );
}

/* ================== ATTACKER MAIN ================= */

/* Untrusted main function to create/enter the trusted enclave. */
int main( int argc, char **argv )
{
    sgx_launch_token_t token = {0};
    int apic_fd, updated = 0, res;
    idt_t idt = {0};

    info_event("Creating enclave...");
    SGX_ASSERT( sgx_create_enclave( "./Enclave/encl.so", /*debug=*/DBG_ENCL,
                                    &token, &updated, &eid, NULL ) );
    /* 0. dry run */
    SGX_ASSERT( dummy_enclave_fn(eid, &res) );

    /* 1. Setup attack execution environment. */
    register_symbols("./Enclave/encl.so");
    attacker_config_runtime();
    attacker_config_page_table();
    register_aep_cb(aep_cb_func);
    
    /* ===================== Tracer usage example ======================== */

    //info_event("Initializing tracer...");
    sgx_tracer_init(&tracer);
 
    //void *pages[] = {code_adrs};
    //sgx_tracer_init_mod(&tracer, /*option=*/TRACK_PAGES, pages, /*num_of_pages=*/1);

    /* Tested: add_module and init for IRQ and GPRS */ 
    /* NOTE: ONLY TRACE GPRS IN DEBUG ENCLAVES, PRODUCTION ENCLAVES DO *NOT* ALLOW THIS*/
    //enum gprsgx_offset regs[] = {RIP};
    //sgx_tracer_init_mod(&tracer, /*option=*/TRACK_GPRS, regs, /*num_of_regs=*/1);
   
    //sgx_tracer_init_mod(&tracer, /*option=*/TRACK_IRQ, NULL, /*number_doesn't_matter=*/0);
    

    /* Add module to add entire module */
    sgx_tracer_add_module(&tracer, /*option=*/TRACK_GPRS | TRACK_IRQ | TRACK_PAGES);

    /* ================================================================= */

    info_event("Establishing user-space APIC/IDT mappings");
    map_idt(&idt);
    install_kernel_irq_handler(&idt, __ss_irq_handler, IRQ_VECTOR);
    apic_timer_oneshot(IRQ_VECTOR);

    ASSERT( !mprotect( (void*) (((uint64_t) code_adrs) & ~PFN_MASK), 0x1000, PROT_NONE ));
    SGX_ASSERT( dummy_enclave_fn(eid, &res) );

    /* ============== Tracer vcd output example & destory tracer =================== */

    info_event("Printing vcd...");
    sgx_tracer_vcd(&tracer, "trace.vcd");
    
    info_event("Destroying tracer...");
    sgx_tracer_destroy(&tracer); 

    return 0;
}
