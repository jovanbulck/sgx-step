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

    .data
    .global secret_str
    .align 0x1000   /* 4KiB */
secret_str:
    .ascii "SysTEX 2017" //len=11
    .space 0x100000 /* 1 MiB */

#ifdef M32
    .macro lea_pic symbol, reg
        call  . + 0x5
        pop   \reg
        lea   (\symbol - . + 1)(\reg), \reg
    .endm
#else
    .macro lea_pic symbol, reg
        lea   \symbol(%rip), \reg
    .endm
#endif


    .text
    .global my_strlen
    .align 0x1000   /* 4KiB */
    .type my_strlen,@function
my_strlen:
    mov    %rdi,%rax
1:  cmpb   $0x0,(%rax)
    je     2f
    inc    %rax
    jmp    1b
2:  sub    %rdi,%rax
    retq   
    .space 0x1000   /* 4KiB */

    .text
    .global zigzag_bench
    .align 0x1000   /* 4KiB */
    .type zigzag_bench,@function
// code snippet from
// https://www.usenix.org/system/files/conference/usenixsecurity17/sec17-lee-sangho.pdf
#ifdef M32

zigzag_bench:
            cmp $0, 0x8(%ebp)
            je zigzag_bench_ret

block0:     lea_pic block1, %edx
            lea_pic block2, %ecx
            lea_pic a, %eax
            cmp $0, (%eax)
            cmove %ecx, %edx
block0.j:   jmp zz1
block1:     nop
            lea_pic block5, %edx
block1.j:   jmp zz2
block2:     lea_pic block3, %edx
            lea_pic block4, %ecx
            lea_pic b, %eax
            cmp $0, (%eax)
            cmove %ecx, %edx
block2.j:   jmp zz3
block3:     nop
            lea_pic block5, %edx
block3.j:   jmp zz4
block4:     nop
block5:     nop

            decl 0x8(%ebp)
            jmp zigzag_bench

zigzag_bench_ret:
            ret

zz1:        jmp block1.j
zz2:        jmp block2.j
zz3:        jmp block3.j
zz4:        jmp *%edx

#else

zigzag_bench:
            cmp $0, %rdi
            je zigzag_bench_ret

block0:     lea_pic block1, %r15
            lea_pic block2, %r14
            cmp $0, a(%rip)
            cmove %r14, %r15
block0.j:   jmp zz1
block1:     nop
            lea_pic block5, %r15
block1.j:   jmp zz2
block2:     lea_pic block3, %r15
            lea_pic block4, %r14
            cmp $0, b(%rip)
            cmove %r14, %r15
block2.j:   jmp zz3
block3:     nop
            lea_pic block5, %r15
block3.j:   jmp zz4
block4:     nop
block5:     nop

            dec %rdi
            jmp zigzag_bench

zigzag_bench_ret:
            retq

zz1:        jmp block1.j
zz2:        jmp block2.j
zz3:        jmp block3.j
zz4:        jmpq *%r15

#endif
