#define IA32_TIME_STAMP_COUNTER 0x10
#define IA32_APIC_BASE          0x1b

    .text
    .global call_gate_func
call_gate_func:
    # store CPL (i.e., ring 0)
    call get_cpl
    mov %rax, gate_cpl(%rip)

    # store privileged RDMSR result
    mov $IA32_APIC_BASE, %rdi
    call rdmsr
    mov %rax, gate_msr(%rip)

    # return to user space caller
    lretq
