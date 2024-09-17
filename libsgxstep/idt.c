#include "idt.h"
#include "pt.h"
#include "apic.h"
#include "sched.h"
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sched.h>

/* See irq_entry.S to see how these are used. */
void __ss_irq_gate(void);
extern exec_priv_cb_t __ss_irq_gate_cb;

uint64_t sgx_step_isr_kernel_map_offset = 0;

/* this externs will be created by the compiler pointing to the ISR section */
extern void *__start_isr_section;
extern void *__stop_isr_section;

extern int fd_step;

static int is_in_isr_section(void *handler)
{
    return (void*) (&__start_isr_section) <= handler && handler < (void*)(&__stop_isr_section);
}

static void setup_isr_map(void)
{
    setup_isr_map_t param;

    param.isr_start = (uint64_t) &__start_isr_section;
    param.isr_stop = (uint64_t) &__stop_isr_section;
    libsgxstep_info("setting up ISR kernel mapping: %#lx-%#lx",
        param.isr_start, param.isr_stop);

    step_open();
    ASSERT( ioctl(fd_step, SGX_STEP_IOCTL_SETUP_ISR_MAP, &param) >= 0 );
    sgx_step_isr_kernel_map_offset = param.isr_kernel_base - param.isr_start;
    libsgxstep_info("received kernel base address %#lx with offset %#lx",
        param.isr_kernel_base, sgx_step_isr_kernel_map_offset);

    /*
     * We currently use one section for both data and code. Therefore, we need
     * executable and writable pages which are per definition a security risk.
     * Linux detects this and removes one of the flags, therefore we directly
     * modify the page table entries here.
     */
    for (uint64_t addr = param.isr_start; addr < param.isr_stop; addr += 0x1000) {
        void *page = (void*) addr + sgx_step_isr_kernel_map_offset;
        uint64_t* pte = remap_page_table_level(page, PTE);
        *pte = MARK_EXECUTABLE(*pte);
        *pte = MARK_WRITABLE(*pte);
        flush_tlb(page);
    }
}

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

    /*
     * NOTE: Linux uses the same physical memory for every IDT on all CPUs (set
     * by `lidt` by Linux at boot time). Hence, any change made in the IDT will
     * be reflected globally across all CPUs.
     */
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
    void *handler = asm_handler;

    /*
     * If the handler is within the special ISR section, we rebase it to the
     * virtual shadow mapping in the global kernel address range.
     */
    if (is_in_isr_section(asm_handler))
    {
        if (sgx_step_isr_kernel_map_offset == 0)
        {
            setup_isr_map();
        }
        handler += sgx_step_isr_kernel_map_offset;
        libsgxstep_info("using kernel-mapped ISR handler: %p -> %p", asm_handler, handler);
    }

    gate_desc_t *gate = gate_ptr(idt->base, vector);
    gate->offset_low    = PTR_LOW(handler);
    gate->offset_middle = PTR_MIDDLE(handler);
    gate->offset_high   = PTR_HIGH(handler);

    gate->p = 1;
    gate->segment = seg;
    gate->dpl = USER_DPL;
    gate->type = type;
    gate->ist = 0;

    libsgxstep_info("installed asm IRQ handler at %x:%p", seg, handler);
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

void install_priv_gate(void *asm_handler, int vector)
{
    idt_t idt;
    libsgxstep_info("locking user-space IRQ gate handler page at %p", __ss_irq_gate);
    ASSERT( !mlock(__ss_irq_gate, 0x1000) );

    libsgxstep_info("installing ring-0 IRQ gate");
    ASSERT( !claim_cpu(VICTIM_CPU) );
    map_idt(&idt);
    /*
     * In principle, we could use a trap gate to make the exec_priv code
     * interruptible, but it seems the Linux kernel does not expect and
     * freezes when interrupting ring-0 code. So we use an interrupt gate
     * here to make the exec_priv code uninterruptible.
     */
    install_kernel_irq_handler(&idt, asm_handler, vector);
    free_map(idt.base);
}

void exec_priv(exec_priv_cb_t cb)
{
    if (!__ss_irq_gate_cb)
    {
        install_priv_gate(__ss_irq_gate, IRQ_PRIV_VECTOR);
        ASSERT( !mlock(&__ss_irq_gate_cb, 0x1000) );
    }

    __ss_irq_gate_cb = cb;
    trigger_sw_irq();
}
