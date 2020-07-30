/* ********************************************************************** */
    .data
    .align 0x1000
    .global __ss_irq_fired, __ss_irq_count, __ss_irq_cpl, apic_base, nemesis_tsc_aex
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

/* ********************************************************************** */
    .text
    .align 0x1000
    .global __ss_irq_handler
__ss_irq_handler:
    /* Nemesis IRQ latency timestamp */
    mov %rax, __ss_irq_rax(%rip)
    mov %rdx, __ss_irq_rdx(%rip)
    rdtsc
    mov %eax, nemesis_tsc_aex(%rip)

    /* IRQ bookkeeping */
    incl __ss_irq_fired(%rip)
    incl __ss_irq_count(%rip)
    mov %cs,  __ss_irq_cpl(%rip)
    and $0x3, __ss_irq_cpl(%rip)

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
    .global sgx_step_irq_gate_func
sgx_step_irq_gate_func:
    call *sgx_step_irq_gate_cb(%rip)
    iretq
