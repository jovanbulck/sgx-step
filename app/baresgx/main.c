#include <stdio.h>
#include <signal.h>
#include "baresgx/urts.h"
#include "libsgxstep/debug.h"
#include "libsgxstep/enclave.h"
#include "libsgxstep/pt.h"
#include "libsgxstep/apic.h"
#include "libsgxstep/idt.h"
#include "libsgxstep/cache.h"
#include "libsgxstep/elf_parser.h"

#define ENCLAVE_PATH            "enclave/encl.elf"
#define ENCLAVE_DBG             1
#define SINGLE_STEP_ENABLE      1

void *encl_page = NULL;
uint64_t *pte_encl = 0;
int do_step = 0, step_cnt = 0, zero_cnt = 0;

void aep_cb_func(void)
{
    uint64_t erip = edbgrd_erip() - (uint64_t)get_enclave_base();
    info("^^ enclave RIP=%#lx; ACCESSED=%lu", erip, ACCESSED(*pte_encl));

    if (do_step && ACCESSED(*pte_encl)) step_cnt++;
    else if (do_step) zero_cnt++;

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
    *pte_encl = MARK_NOT_ACCESSED(*pte_encl);
    flush(pte_encl);

    if (do_step)
    {
        apic_timer_irq(SGX_STEP_TIMER_INTERVAL);
    }
}

void handle_fault(int sig)
{
    info("caught signal %d; restoring trigger page access rights", sig);
    *pte_encl = MARK_EXECUTABLE(*pte_encl);
    do_step = 1;
}

int main(void)
{
    void *tcs;
    uint64_t rv;
    idt_t idt = {0};

    /************************************************************************/
    info_event("loading baresgx enclave");
    tcs = baresgx_load_elf_enclave(ENCLAVE_PATH, ENCLAVE_DBG);
    print_enclave_info();
    register_symbols(ENCLAVE_PATH);

    info("dry run");
    rv = baresgx_enter_enclave(tcs, /*arg=*/0);
    printf("\tL enclave returned %lx\n", rv);

    /************************************************************************/
    info_event("configuring attacker runtime");
    register_aep_cb(aep_cb_func);
    ASSERT( signal(SIGSEGV, handle_fault) != SIG_ERR);

    encl_page = get_symbol_offset("encl_entry") + get_enclave_base();
    info("entry page at %p", encl_page);
    ASSERT(pte_encl = remap_page_table_level(encl_page, PTE));
    ASSERT(PRESENT(*pte_encl));
    print_pte(pte_encl);
    #if SINGLE_STEP_ENABLE
        *pte_encl = MARK_EXECUTE_DISABLE(*pte_encl);

        map_idt(&idt);
        install_kernel_irq_handler(&idt, __ss_irq_handler, IRQ_VECTOR);
        apic_timer_oneshot(IRQ_VECTOR);

        __ss_irq_fired = 0;
        apic_timer_irq( SGX_STEP_TIMER_INTERVAL );
        while (!__ss_irq_fired);
        info("APIC timer IRQ handler seems to be working");
    #endif

    /************************************************************************/
    info_event("single-stepping baresgx enclave");
    rv = baresgx_enter_enclave(tcs, /*arg=*/0);
    info("enclave returned %lx; step_cnt=%d; zero_cnt=%d\n", rv, step_cnt, zero_cnt);

    return 0;
}
