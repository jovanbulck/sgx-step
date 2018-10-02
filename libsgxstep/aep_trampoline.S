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
    mov %xbx, sgx_step_tcs

    /* optional C function callback */
    lea sgx_step_aep_cb, %xax
    mov (%xax), %xax
    test %xax, %xax
    je .Leresume
    call *%xax

.Leresume:
    mov sgx_step_tcs, %xbx              /* TCS address */
    lea sgx_step_aep_trampoline, %xcx   /* AEP address */

    prefetch (nemesis_tsc_eresume)
    mfence

    rdtsc
    mov %eax, (nemesis_tsc_eresume)

    mov $3, %xax                        /* ERESUME leaf */
    .byte 0x0f, 0x01, 0xd7              /* ENCLU */
