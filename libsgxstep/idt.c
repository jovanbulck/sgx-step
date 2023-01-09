#include "idt.h"
#include "pt.h"
#include "apic.h"
#include "sched.h"
#include <sys/mman.h>
#include <sys/ioctl.h>
  #include <sched.h>

/* See irq_entry.S to see how these are used. */
void sgx_step_irq_gate_func(void);
exec_priv_cb_t sgx_step_irq_gate_cb = NULL;

uint64_t sgx_step_isr_kernel_map_offset = 0;

// this externs will be created by the compiler pointing to the ISR section.
extern void *__start_isr_section;
extern void *__stop_isr_section;

extern int fd_step;

static int is_in_isr_section(void *handler) {
    return (void*)(&__start_isr_section) <= handler && handler < (void*)(&__stop_isr_section);
}

static void setup_isr_map()
{
    setup_isr_map_t param;

    param.isr_start = (uint64_t)&__start_isr_section;
    param.isr_stop = (uint64_t)&__stop_isr_section;

    info("setting up isr mapping: from 0x%lx to 0x%lx", param.isr_start, param.isr_stop);

    step_open();
    ASSERT( ioctl(fd_step, SGX_STEP_IOCTL_SETUP_ISR_MAP, &param) >= 0 );

    info("we received the base address from kernel 0x%lx", param.isr_kernel_base);

    // calculate the virtual address space offset
    sgx_step_isr_kernel_map_offset = param.isr_kernel_base - param.isr_start;

    info("the offset to the kernel mapped ISR region is 0x%lx", sgx_step_isr_kernel_map_offset);

    // This is currently a workaround: We are currently use one section for both data and code.
    // Therefore, we need executable and writable pages which are per definition a security risk.
    // Linux detects this and removes one of the flags, therefore we directly modify the page tables.
    for (uint64_t address = param.isr_start; address < param.isr_stop; address += 0x1000) {
        void *page = (void*)address + sgx_step_isr_kernel_map_offset;
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

    uint64_t handler = (uint64_t)asm_handler;

    // check if the ISR section is mapped
    if (sgx_step_isr_kernel_map_offset == 0) {
        setup_isr_map();
    }

    // only if the handler is within the ISR section we use the kernel mapped handler
    if (is_in_isr_section(asm_handler)) {
        handler += sgx_step_isr_kernel_map_offset;
        libsgxstep_info("using kernel mapped ISR handler: %p -> %p", asm_handler, (void*)handler);
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
    // TODO: maybe call setup_isr_map here? this would require to open sgx_step in the constructor.
}
