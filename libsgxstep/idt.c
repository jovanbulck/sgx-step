#include "idt.h"
#include "pt.h"
#include "apic.h"
#include "sched.h"
#include <sys/mman.h>

/* See irq_entry.S to see how these are used. */
void sgx_step_irq_gate_func(void);
exec_priv_cb_t sgx_step_irq_gate_cb = NULL;

void dump_gate(gate_desc_t *gate, int idx)
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
    entries = (idtr.size+1)/sizeof(gate_desc_t);
    dump_dtr(&idtr, entries);

    ASSERT( idtr.base );
    idt_base = remap_page_table_level((void*) idtr.base, PAGE);
    libsgxstep_info("established user space IDT mapping at %p", idt_base);
    ASSERT(idt_base);

    idt->base = (gate_desc_t*) idt_base;
    idt->entries = entries;
}

void install_irq_handler(idt_t *idt, void* asm_handler, int vector, cs_t seg, gate_type_t type)
{
    ASSERT(vector >= 0 && vector < idt->entries);

    gate_desc_t *gate = gate_ptr(idt->base, vector);
    gate->offset_low    = PTR_LOW(asm_handler);
    gate->offset_middle = PTR_MIDDLE(asm_handler);
    gate->offset_high   = PTR_HIGH(asm_handler);

    gate->p = 1;
    gate->segment = seg;
    gate->dpl = USER_DPL;
    gate->type = type;
    gate->ist = 0;

    libsgxstep_info("installed asm IRQ handler at %x:%p", seg, asm_handler);
    #if !LIBSGXSTEP_SILENT
        dump_gate(gate, vector);
    #endif
}

void install_user_irq_handler(idt_t *idt, void* asm_handler, int vector)
{
    /*
     * Note we explicitly use a trap gate here and not an interrupt gate,
     * since the Interrupt Enable (IE) flag won't get reset on iretq in user
     * space, resulting in a stalled CPU.
     */
    install_irq_handler(idt, asm_handler, vector, USER_CS, GATE_TRAP);
}

void install_kernel_irq_handler(idt_t *idt, void *asm_handler, int vector)
{
    /* We can use an interrupt gate to make the ring0 handler uninterruptible. */
    install_irq_handler(idt, asm_handler, vector, KERNEL_CS, GATE_INTERRUPT);
}

void exec_priv(exec_priv_cb_t cb)
{
    idt_t idt;
    if (!sgx_step_irq_gate_cb)
    {
        libsgxstep_info("installing and calling ring0 irq gate");
        ASSERT( !claim_cpu(VICTIM_CPU) );
        map_idt(&idt);
        /* We use a trap gate to make the exec_priv code interruptible. */
        install_irq_handler(&idt, sgx_step_irq_gate_func, IRQ_VECTOR+4, KERNEL_CS, GATE_TRAP);
        free_map(idt.base);
    }

    sgx_step_irq_gate_cb = cb;
    asm("int %0\n\t" ::"i"(IRQ_VECTOR+4):);
}

void __attribute__((constructor)) init_sgx_step( void )
{
    /* Ensure IRQ handler asm code is not subject to demand-paging */
    info("locking IRQ handler pages %p/%p", &__ss_irq_handler, &__ss_irq_fired);
    ASSERT( !mlock(&__ss_irq_handler, 0x1000) );
    ASSERT( !mlock((void*) &__ss_irq_fired, 0x1000) );

    print_page_table(__ss_irq_handler);
    print_page_table(init_sgx_step);
}
