#define APIC_ICR_LOW                0x300
#define APIC_ICR_HIGH               0x310
#define APIC_ICR_MSR                0x830
#include "libsgxstep/config.h"

.macro apic_write_icr
#if !X2APIC
    mov apic_base(%rip), %rax
    movl %esi, APIC_ICR_HIGH(%rax)
    /* SDM: The act of writing to the low doubleword of the ICR causes the IPI to be sent. */
    movl %edi, APIC_ICR_LOW(%rax)
#else /* X2APIC */
    /* SDM: A single MSR write to the Interrupt Command Register is required for dispatching an interrupt in x2APIC mode. */
    shl $32, %rsi
    or  %rdi, %rsi
    mov $APIC_ICR_MSR, %rdi
    int $WRMSR_GATE_VECTOR
#endif /* X2APIC */
.endm

    .text
    .global apic_write_icr_ret
apic_write_icr_ret:
    apic_write_icr
    ret

    .global apic_write_icr_nop
apic_write_icr_nop:
    apic_write_icr

    /* include a NOP slide to measure IPI self latency */
    .global apic_write_nop_slide_start
apic_write_nop_slide_start:
    .rept 1000000
        nop
    .endr
    .global apic_write_nop_slide_end
apic_write_nop_slide_end:
    nop

    retq
