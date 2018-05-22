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

/* 
 * ---------------------------------------------------------------------
 * Function: sgx_step_trampoline
 *      Code that will be returned to after interrupting an enclave.
 *
 * Registers:
 *      RAX - ERESUME leaf
 *      RBX - TSC address
 *      RCX - AEP address
 */
    .text
    .global sgx_step_aep_trampoline
    .type sgx_step_aep_trampoline, @function
sgx_step_aep_trampoline:
    mov %rbx, sgx_step_tcs

    /* optional C function callback */
    mov (sgx_step_aep_cb), %rax
    test %rax, %rax
    je .Leresume
    mov sgx_step_erip, %rdi
    call *%rax
    movq $0, sgx_step_erip

.Leresume:
    mov $3, %rax                        /* ERESUME leaf */
    mov sgx_step_tcs, %rbx              /* TCS address */
    lea sgx_step_aep_trampoline, %rcx   /* AEP address */
    .byte 0x0f, 0x01, 0xd7              /* ENCLU */
