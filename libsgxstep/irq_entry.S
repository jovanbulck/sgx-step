/*
 * NOTE: code/data in the dedicated `isr_section` will be mapped by libsgxstep
 * into the global kernel address range. This ensures that the handler is always
 * available across CR3 switches (as the APIC timer may sometimes fire after
 * the libsgxstep process has been context switched).
 *
 * Important considerations when programming APIC interrupt handlers:
 *
 *      1. All code and global data referenced in the handler should be placed
 *         in the special `isr_section`.
 *      2. All code should be written position-independent.
 *
 */
/* ********************************************************************** */
    .section isr_section,"awx",@progbits
    .align 0x1000
    .global __ss_irq_fired, __ss_irq_count, __ss_irq_cpl, apic_base, nemesis_tsc_aex, __ss_irq_rip
__ss_irq_fired:
    .int 0x0
__ss_irq_count:
    .int 0x0
__ss_irq_cpl:
    .int 0xff
apic_base:
    .quad 0x0
nemesis_tsc_aex:
    .quad 0x0;
    /* not sure there's a kernel stack we can use(?) */
__ss_irq_rax:
    .quad 0x0
__ss_irq_rdx:
    .quad 0x0
__ss_irq_rip:
    .quad 0x0

/* ********************************************************************** */
    .section isr_section,"awx",@progbits
    .align 0x1000
    .global __ss_irq_handler
__ss_irq_handler:
    /* Nemesis IRQ latency timestamp */
    mov %rax, __ss_irq_rax(%rip)
    mov %rdx, __ss_irq_rdx(%rip)
    rdtsc
    mov %eax, nemesis_tsc_aex(%rip)
    mov %edx, nemesis_tsc_aex+4(%rip)

    /* IRQ bookkeeping */
    mov 0(%rsp), %rax
    mov %rax, __ss_irq_rip(%rip)
    movw %cs,  __ss_irq_cpl(%rip)
    andw $0x3, __ss_irq_cpl(%rip)
    incl __ss_irq_count(%rip)
    incl __ss_irq_fired(%rip)

    /* apic_write(APIC_EOI, 0x0); */
    lea apic_base(%rip), %rax
    mov (%rax),%rax
    test %rax, %rax
    jz 1f
    add $0xb0, %rax
    movl $0x0, (%rax)
1:
    mov __ss_irq_rax(%rip), %rax
    mov __ss_irq_rdx(%rip), %rdx
    iretq

/* ********************************************************************** */
/*
 * NOTE: The following handler is only ever supposed to be triggered from
 * within the libsgxstep process (CR3) by means of a dedicated software
 * interrupt `int` instruction. Hence, we don't have to map it into the global
 * kernel virtual address range.
 */
    .text
    .align 0x1000
    .global __ss_irq_gate
__ss_irq_gate:
    call *__ss_irq_gate_cb(%rip)
    iretq
