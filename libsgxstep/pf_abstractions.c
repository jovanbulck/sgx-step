#include "pf.h"
#include "pt_abstractions.h"
#include "idt.h"
#include "cpu.h"
#include "sched.h"
#include "enclave.h"
#include "pf_abstractions.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

volatile fault_handler_t __idt_fault_handler_cb = NULL;

void callback_wrapper_func(void)
{
    uint64_t addr = __pf_irq_faultaddr;
    if (addr)
    {
        size_t pagenum = virt_to_pagenum((void *)addr);
        __pf_irq_faultaddr = 0; // Set it to 0
        __idt_fault_handler_cb(pagenum);
    } // There is no pagefault that occured due to the SGX bit having been set
}

void setup_IDT_entry(void)
{
    idt_t idt = {0};
    map_idt(&idt);
    void *original_gate_ptr = (void *)gate_offset(gate_ptr((&idt)->base, 14));
    __pf_irq_original_handler_addr = (uint64_t)original_gate_ptr;
    install_kernel_irq_handler(&idt, __pf_irq_handler, 14);
}

void register_fault_handler_IDT(fault_handler_t cb)
{
    setup_IDT_entry();
    __idt_fault_handler_cb = cb;
    register_aep_pf_cb(callback_wrapper_func);
}