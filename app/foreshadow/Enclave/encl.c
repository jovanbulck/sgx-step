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

extern uint8_t *trigger_page;

void* enclave_get_trigger_adrs(void)
{
    return &trigger_page;
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
        : "S" (&trigger_page)
        : "rax", "rbx", "rcx", "rdx", "rdi",
          "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
    );
}
