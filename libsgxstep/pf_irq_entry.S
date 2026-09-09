.section isr_section,"awx",@progbits
.align 0x1000
.global __pf_irq_original_handler_addr, __pf_irq_faultaddr
__pf_irq_original_handler_addr:
    .quad 0x0    

__pf_irq_faultaddr:
    .quad 0x0

.section isr_section,"awx",@progbits
.align 0x1000
.global __pf_irq_handler
__pf_irq_handler:

    # Save registers
    push %rax
    pushfq

    # Load the error code
    mov 16(%rsp), %rax

    # Check if the SGX bit (Bit 15) is set
    # need to save the rflags register as well
    bt $15, %rax
    jc 1f
    # test $0x8000, %rax # Alternative way of checking bit 15
    # jnz 1f

    # SGX bit isn't set
    # Restore registers
    popfq
    pop %rax

    # Call Original handler
    jmp *__pf_irq_original_handler_addr(%rip)


1:
    /*
    We only reach this code whenever the SGX bit is set

    Just return without calling a handler, we register the handler 
    in the aep callback, just need to indicate to the aep callback where the
    pf occured. Currently doing so via __pf_irq_faultaddr
    */ 

    # Read the faulting addr (NOT THE SAME AS FAULTING INSTRUCTION ADDR)
    movq %cr2, %rax
    movq %rax, __pf_irq_faultaddr(%rip)

    # Restore everything and return
    popfq
    pop %rax
    add $8, %rsp  # Remove error code from the stack
    iretq
