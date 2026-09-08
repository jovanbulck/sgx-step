
#include "config.h"
/*
 *  This file is part of the SGX-Step enclave execution control framework.
 *
 *  Copyright (C) 2017 Jo Van Bulck <jo.vanbulck@cs.kuleuven.be>,
 *                     Raoul Strackx <raoul.strackx@cs.kuleuven.be>
 *
 *  SGX-Step is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SGX-Step is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SGX-Step. If not, see <http://www.gnu.org/licenses/>.
 */

/* 32-bit compatibility ~~following Intel SGX-SDK convention. */
#if defined(M32)
    #define xax  eax
    #define xbx  ebx
    #define xcx  ecx
    #define xdi  edi
    #define movx movl
#else
    #define xax  rax
    #define xbx  rbx
    #define xcx  rcx
    #define xdi  rdi
    #define movx movq
#endif

/* 
 * ---------------------------------------------------------------------
 * Function: sgx_step_trampoline
 *      Code that will be returned to after interrupting an enclave.
 *
 * Registers:
 *      XAX - ERESUME leaf
 *      XBX - TSC address
 *      XCX - AEP address
 */

    .text
    .global sgx_step_aep_trampoline
    .type sgx_step_aep_trampoline, @function
sgx_step_aep_trampoline:
    /*
     * Save TCS address thread-local on the stack
     * NOTE: maintain 16-byte stack alignment (ABI calling convention)
     */
    push %xbx
    mov %rsp, %rbp
    and $~0xf, %rsp

    /* optional C function callback */
#ifdef BUILD_SHARED
    mov sgx_step_aep_cb@GOTPCREL(%rip), %xax
#else
    lea sgx_step_aep_cb(%rip), %xax
#endif
    mov (%xax), %xax
    test %xax, %xax
    je .Leresume
    call *%xax

.Leresume:
#ifdef BUILD_SHARED
    mov  sgx_step_eresume_cnt@GOTPCREL(%rip), %r11
    incl (%r11)

    /* restore stack and TCS address */
    mov %rbp, %rsp
    pop %xbx

    mov sgx_step_aep_trampoline@GOTPCREL(%rip), %xcx   /* AEP address */

    mov      nemesis_tsc_eresume@GOTPCREL(%rip), %r11
    prefetch (%r11)
    mfence

    rdtsc

    mov  nemesis_tsc_eresume@GOTPCREL(%rip), %r11
    mov %eax, (%r11)

    mov $3, %xax                        /* ERESUME leaf */

    /* optionally set RFLAGS.TF to enable simulated single-stepping for DEBUG enclaves */
    mov sgx_step_do_trap@GOTPCREL(%rip), %rdx
    mov (%rdx), %rdx

    test %rdx, %rdx
    je sgx_step_aep_eresume
    pushf
    orl $0x100, (%rsp)
    popf

#else
    incl sgx_step_eresume_cnt(%rip) //increment what is at that address

    /* restore stack and TCS address */
    mov %rbp, %rsp
    pop %xbx
    
    lea sgx_step_aep_trampoline(%rip), %xcx   /* AEP address */

    prefetch nemesis_tsc_eresume(%rip)
    mfence

    rdtsc
    mov %eax, nemesis_tsc_eresume(%rip)

    mov $3, %xax                        /* ERESUME leaf */

    /* optionally set RFLAGS.TF to enable simulated single-stepping for DEBUG enclaves */
    lea sgx_step_do_trap(%rip), %rdx
    mov (%rdx), %rdx
    test %rdx, %rdx
    je sgx_step_aep_eresume
    pushf
    orl $0x100, (%rsp)
    popf
#endif

.global sgx_step_aep_eresume
sgx_step_aep_eresume:
    .byte 0x0f, 0x01, 0xd7              /* ENCLU */
    ud2

#ifdef BUILD_SHARED
.global eenter_stub 
eenter_stub:
    mov aep_pt@GOTPCREL(%rip), %rcx
    mov (%rcx), %rcx

    /* Check if i should set apic timer here */
    mov sgx_step_do_irq@GOTPCREL(%rip), %r8
    mov (%r8), %r8
    test %r8, %r8
    je .Lnotimer
    
    /* protect regs */     
    push %rbp
    mov %rsp, %rbp
    push %xbx
    push %xax
    push %xcx

    /* Set up apic timer before eenter */
    mov apic_timer_irq_asm_wrapper@GOTPCREL(%rip), %r11
    mov $SGX_STEP_TIMER_INTERVAL, %rdi
    call *%r11

    /* restore stack */
    pop %xcx
    pop %xax
    pop %xbx

    mov %rbp, %rsp
    pop %rbp

    jmp .Leenter          /* No trap and timer at same time */

.Lnotimer:
    /* optionally set RFLAGS.TF to enable simulated single-stepping for DEBUG enclaves */
    mov sgx_step_do_trap@GOTPCREL(%rip), %rdx
    mov (%rdx), %rdx

    test %rdx, %rdx
    je .Leenter
    pushf
    orl $0x100, (%rsp)
    popf

.Leenter:
    .byte 0x0f, 0x01, 0xd7              /* ENCLU */
    mov enclu_cont_instr@GOTPCREL(%rip), %rax
    mov (%rax), %rax
    jmp *%rax
    ud2

.global eresume_stub 
eresume_stub:
    enclu
    ud2 # should never get here
#endif
