#if !NO_SGX
#include <stddef.h>

#include "debug.h"
#include "enclave.h"
#include "config.h"
#include "pt.h"
#include "apic.h"
#include "cache.h"

extern void sgx_step_aep_eresume(void);

void *tcs_pt = NULL;
void *aep_pt = sgx_step_aep_eresume;

static int irq_cnt = 0, set_pmd = 0, steps = 0;
uint64_t *sgx_step_pmd_encl = NULL;

extern int tf_enabled;
extern int tracer_enabled;
int NUM_RUNS = 1;

extern int eenter_count, target_ecall, skip_ecalls;

/* Custom AEP get/set fn (used to be in patched SDK urts) */
void* get_aep(void)
{
    return aep_pt;
}

void set_aep(void* aep)
{
    info("aep address given %p", aep);
    aep_pt = aep;
}

void* get_tcs(void)
{
    return tcs_pt;
}

#if 0
/* TODO: Build generic aep that even updates tracer */
/* Called before resuming the enclave after an Asynchronous Enclave eXit. */
void sgx_step_aep_cb_func(void)
{

    /* Guard to check if i should track ecall */
    if ( target_ecall != eenter_count && skip_ecalls >= eenter_count && target_ecall != -2 )
    {
        return;
    }
    
    if ( !set_pmd )
    {
        ASSERT( sgx_step_pmd_encl = remap_page_table_level( get_enclave_base(), PMD) );
        ASSERT( PRESENT(*sgx_step_pmd_encl) );
        set_pmd = 1; 
    }

    if ( tf_enabled )
        DISABLE_TF;

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
    //if (ACCESSED(*pte_encl)) // change this such that it is any enclave page is accessed
    if (is_enclave_exec_accessed())
    {
        if ( tracer_enabled ) sgx_tracer_step(&tracer);
    }


    // TODO: tracer clears accessed bit but 
    // if tracer is not enabled we have to clear 
    // exec access bit manually add if guard
    // mark_enclave_exec_not_accessed()

    /*
     * Configure APIC timer interval for next interrupt.
     *
     * On our evaluation platforms, we explicitly clear the enclave's
     * _unprotected_ PMD "accessed" bit below, so as to slightly slow down
     * ERESUME such that the interrupt reliably arrives in the first subsequent
     * enclave instruction.
     * 
     */
    *sgx_step_pmd_encl = MARK_NOT_ACCESSED( *sgx_step_pmd_encl );
    flush(sgx_step_pmd_encl);

    if ( !tf_enabled )
        apic_timer_irq( SGX_STEP_TIMER_INTERVAL );
}

#endif

void sgx_step_aep_cb_func_placeholder(void)
{
    if ( tf_enabled )
        DISABLE_TF;

    /* Guard to check if i should track ecall */
    if ( target_ecall != eenter_count && skip_ecalls >= eenter_count && target_ecall != -2 )
    {
        return;
    }
    info("Hello from the aep placeholder in aep.c");
}
#endif
