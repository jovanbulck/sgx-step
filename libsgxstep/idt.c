#include "idt.h"
#include "pt.h"
#include "apic.h"

/* See irq_entry.S to see how these are used. */
extern void sgx_step_irq_entry(void);
irq_cb_t sgx_step_irq_cb_table[256] = {0};
uint8_t sgx_step_vector_hack = 0;

void dump_gate(idt_gate_t *gate, int idx)
{
    info("IDT[%3d] @%p = %p (seg sel 0x%x); p=%d; dpl=%d; type=%02d; ist=%d",
        idx, gate, (void*) gate_offset(gate), gate->segment, gate->p, gate->dpl, gate->type, gate->ist);
}

void dump_idt(idt_t *idt)
{
    int i;

    info("--------------------------------------------------------------------------------");
    for (i =0; i < idt->entries; i++)
        dump_gate(gate_ptr(idt->base, i), i);
    info("--------------------------------------------------------------------------------");
}

void map_idt(idt_t *idt)
{
    dtr_t idtr = {0};
    void *idt_base = NULL;
    int entries;

    asm volatile ("sidt %0\n\t"
                  :"=m"(idtr) :: );
    entries = (idtr.size+1)/sizeof(idt_gate_t);
    dump_dtr(&idtr, entries);

    ASSERT( idtr.base );
    idt_base = remap_page_table_level((void*) idtr.base, PAGE);
    info("established user space IDT mapping at %p", idt_base);
    ASSERT(idt_base);

    idt->base = (idt_gate_t*) idt_base;
    idt->entries = entries;
}

void sgx_step_irq_handler_c(uint8_t *rsp, uint8_t vector)
{
    if (sgx_step_irq_cb_table[vector])
        sgx_step_irq_cb_table[vector](rsp);

    apic_write(APIC_EOI, 0x0);
}

void install_user_irq_handler(idt_t *idt, irq_cb_t handler, int vector)
{
    ASSERT(vector >= 0 && vector < idt->entries);

    idt_gate_t *gate = gate_ptr(idt->base, vector);
    gate->offset_low    = PTR_LOW(sgx_step_irq_entry);
    gate->offset_middle = PTR_MIDDLE(sgx_step_irq_entry);
    gate->offset_high   = PTR_HIGH(sgx_step_irq_entry);
    sgx_step_irq_cb_table[vector] = handler;
    
    ASSERT(!sgx_step_vector_hack &&
            "SGX-Step currently only supports a single user space IRQ vector");
    sgx_step_vector_hack = vector;

    gate->p = 1;
    gate->segment = USER_CS;
    gate->dpl = USER_DPL;
    /*
     * XXX Note we explicitly use a trap gate here and not an interrupt gate,
     * since the Interrupt Enable (IE) flag won't get reset on iretq in user
     * space, resulting in a stalled CPU.
     */
    gate->type = GATE_TRAP;
    gate->ist = 0;

    info("installed IRQ handler with target_rip=%p", handler);
    dump_gate(gate, vector);
}
