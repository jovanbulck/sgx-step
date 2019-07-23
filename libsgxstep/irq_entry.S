    .text
    .global sgx_step_irq_entry
    .type sgx_step_irq_entry,@function
sgx_step_irq_entry:
    rdtsc
    mov %eax, nemesis_tsc_aex(%rip)

    /* HACK: restore edx:eax from AEX synthetic register state */
    mov $3, %rax
    mov $0, %rdx

    push %rax
    mov %rsp, %rax
    add $8,%rax
    push %rbx
    push %rcx
    push %rdx
    push %rbp
    push %rsi
    push %rdi
    push %r8
    push %r9
    push %r10
    push %r11

    mov %rax, %rdi
    /* TODO support 256 possible IRQ handlers via asm macro */
    movb sgx_step_vector_hack(%rip), %sil
    call sgx_step_irq_handler_c

    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rdi
    pop %rsi
    pop %rbp
    pop %rdx
    pop %rcx
    pop %rbx
    pop %rax
    iretq

    .global sgx_step_irq_gate_func
sgx_step_irq_gate_func:
    call *sgx_step_irq_gate_cb(%rip)
    /* interrupt return to user space caller (this will re-enable IRQs) */
    iretq
