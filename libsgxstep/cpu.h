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

#ifndef SGX_STEP_CPU_H
#define SGX_STEP_CPU_H

#include <stdint.h>

#define IA32_APIC_BASE_MSR      0x1b
#define IA32_TSC_DEADLINE_MSR   0x6e0

#define ENABLE_TF \
	__asm__ volatile ("pushf\norl $0x100, (%rsp)\npopf\n")

#define DISABLE_TF \
	__asm__ volatile ("pushf\nandl $0xfffffeff, (%rsp)\npopf\n")

uint64_t rdtsc_begin( void );
uint64_t rdtsc_end( void );
uint64_t read_flags(void);
void clflush(void *p);

int rdmsr_on_cpu(uint32_t reg, int cpu, uint64_t *data);
int wrmsr_on_cpu(uint32_t reg, int cpu, uint64_t data);

uint64_t rdmsr(uint32_t reg);
void wrmsr(uint32_t reg, uint64_t val);

#endif
