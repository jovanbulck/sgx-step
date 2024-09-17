#define IA32_TIME_STAMP_COUNTER 0x10
#define IA32_APIC_ID            0x802

    .text
    .global do_rdmsr
do_rdmsr:
    # store CPL (i.e., ring 0)
    call get_cpl
    mov %rax, gate_cpl(%rip)

    # store privileged RDMSR result
    mov $IA32_APIC_ID, %ecx
    rdmsr
    shl $32, %rdx
    or  %rdx, %rax
    mov %rax, gate_msr(%rip)
    ret

    .global call_gate_func
call_gate_func:
    call do_rdmsr
    # long return to user space caller
    lretq

    .global irq_gate_func
irq_gate_func:
    call do_rdmsr
    # interrupt return to user space caller (this will re-enable IRQs)
    iretq
