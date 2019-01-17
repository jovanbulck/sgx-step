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

#include <stdint.h>
#include <sgx_trts.h>

// read entire cache line
#define CACHE_LINE_SIZE     64
#ifndef SECRET_BYTES
    #define SECRET_BYTES    CACHE_LINE_SIZE
#endif

// first few cache lines seem not to work stable (?)
#define SECRET_CACHE_LINE   27
#define SECRET_OFFSET       (CACHE_LINE_SIZE*SECRET_CACHE_LINE)

uint8_t __attribute__ ((aligned(0x1000))) array[1000];
#define secret array[SECRET_OFFSET]

void *enclave_generate_secret( void )
{
	sgx_read_rand( &secret, SECRET_BYTES);
    return &secret;
}

void enclave_destroy_secret( uint8_t cl[SECRET_BYTES] )
{
    uint8_t rv = secret;

    for (int i=0; i < SECRET_BYTES; i++)
    {
        cl[i] = array[SECRET_OFFSET+i];
        array[SECRET_OFFSET+i] = 0xff;
    }
}

void enclave_reload( void *adrs )
{
    asm volatile (
        "movl (%0), %%eax\n\t"
        : : "c" (adrs)
        : "%rax");
}

void enclave_run(void)
{
    asm volatile (
        "mov $0xdeadbeefcafebabe, %%rax \n\t"
        "mov $0xdabbad00dabbad00, %%rbx \n\t"
        "mov $0xd0000dbaaaaaaaad, %%rcx \n\t"
        "mov $0xdeadc0debaadf00d, %%rdx \n\t"
        "mov $0xdddddddddddddddd, %%rdi \n\t"
        "mov $0x8888888888888888, %%r8  \n\t"
        "mov $0x9999999999999999, %%r9  \n\t"
        "mov $0x1010101010101010, %%r10 \n\t"
        "mov $0x1111111111111111, %%r11 \n\t"
        "mov $0x1212121212121212, %%r12 \n\t"
        "mov $0x1313131313131313, %%r13 \n\t"
        "mov $0x1414141414141414, %%r14 \n\t"
        "mov $0x1515151515151515, %%r15 \n\t"
        "mov (%0), %%rdi \n\t"
        :
        : "S" (&secret)
        : "rax", "rbx", "rcx", "rdx", "rdi",
          "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
    );
}
