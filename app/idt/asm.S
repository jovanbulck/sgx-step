    .data
    .global __ss_irq_fired, __ss_irq_count, __ss_irq_cpl
__ss_irq_fired:
    .int 0x0
__ss_irq_count:
    .int 0x0
__ss_irq_cpl:
    .int 0xff
    /* not sure there's a kernel stack we can use(?) */
__ss_irq_rax:
    .quad 0x0

    .text
    .global __ss_irq_handler
__ss_irq_handler:
    incl __ss_irq_fired(%rip)
    incl __ss_irq_count(%rip)

    mov %cs,  __ss_irq_cpl(%rip)
    and $0x3, __ss_irq_cpl(%rip)

    /* apic_write(APIC_EOI, 0x0); */
    mov %rax, __ss_irq_rax(%rip)
    lea apic_base(%rip), %rax
    mov (%rax),%rax
    test %rax, %rax
    jz 1f
    add $0xb0, %rax
    movl $0x0, (%rax)
1:
    mov __ss_irq_rax(%rip), %rax
    iretq
