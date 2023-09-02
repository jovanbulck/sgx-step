#include "config.h"

/* ********************************************************************** */
    .section isr_section,"awx",@progbits
    .global __apic_irq_tsc, __apic_irq_rax, __apic_deadline_tsc
__apic_irq_tsc:
    .quad 0x0
__apic_deadline_tsc:
    .quad 0x0
__apic_irq_rax:
    .quad 0x0
__apic_irq_rcx:
    .quad 0x0
__apic_irq_rdx:
    .quad 0x0

/* ********************************************************************** */
    .section isr_section,"awx",@progbits
    .global __apic_irq_handler_timer
__apic_irq_handler_timer:
    mov %rax, __apic_irq_rax(%rip)
    mov %rdx, __apic_irq_rdx(%rip)
    rdtsc
    mov %eax, __apic_irq_tsc(%rip)
    mov %edx, __apic_irq_tsc+4(%rip)

    /* IRQ bookkeeping */
    incl __ss_irq_fired(%rip)

    /* apic_write(APIC_EOI, 0x0); */
    lea apic_base(%rip), %rax
    mov (%rax),%rax
    test %rax, %rax
    jz 1f
    add $0xb0, %rax
    movl $0x0, (%rax)
1:
    mov __apic_irq_rax(%rip), %rax
    mov __apic_irq_rdx(%rip), %rdx
    iretq

    .section isr_section,"awx",@progbits
    .global __apic_priv_irq_gate
__apic_priv_irq_gate:
    push %rax
    push %rcx
    push %rdx

    /* wrmsr(IA32_TSC_DEADLINE_MSR, __apic_deadline_tsc) */
    mov __apic_deadline_tsc(%rip), %eax
    mov __apic_deadline_tsc+4(%rip), %edx
    mov $IA32_TSC_DEADLINE_MSR, %ecx
    wrmsr

    pop %rdx
    pop %rcx
    pop %rax
    iretq
