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
    lea sgx_step_aep_cb(%rip), %xax
    mov (%xax), %xax
    test %xax, %xax
    je .Leresume
    call *%xax

.Leresume:
    incl sgx_step_eresume_cnt(%rip)

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

.global sgx_step_aep_eresume
sgx_step_aep_eresume:
    .byte 0x0f, 0x01, 0xd7              /* ENCLU */
    ud2
