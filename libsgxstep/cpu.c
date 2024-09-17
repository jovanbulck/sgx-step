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

#include "cpu.h"
#include "file.h"
#include <stdio.h>

/*
 * Code adapted from
 * https://github.com/IAIK/flush_flush/blob/master/sc/cacheutils.h
 */
uint64_t rdtsc_begin( void )
{
  uint64_t begin;
  uint32_t a, d;

  asm volatile (
    "mfence\n\t"
    "CPUID\n\t"
    "RDTSCP\n\t"
    "mov %%edx, %0\n\t"
    "mov %%eax, %1\n\t"
    "mfence\n\t"
    : "=r" (d), "=r" (a)
    :
    : "%eax", "%ebx", "%ecx", "%edx");

  begin = ((uint64_t)d << 32) | a;
  return begin;
}

uint64_t rdtsc_end( void )
{
  uint64_t end;
  uint32_t a, d;

  asm volatile(
    "mfence\n\t"
    "RDTSCP\n\t"
    "mov %%edx, %0\n\t"
    "mov %%eax, %1\n\t"
    "CPUID\n\t"
    "mfence\n\t"
    : "=r" (d), "=r" (a)
    :
    : "%eax", "%ebx", "%ecx", "%edx");

  end = ((uint64_t)d << 32) | a;
  return end;
}

/*
 * Code adapted from <https://github.com/01org/msr-tools/>.
 * requires 'msr' Linux kernel module (try `sudo modprobe msr`)
 */
int rdmsr_on_cpu(uint32_t reg, int cpu, uint64_t *data)
{
    char msr_file_name[64];
    
    sprintf(msr_file_name, "/dev/cpu/%d/msr", cpu);
    return file_read_offset(msr_file_name, (uint8_t*) data, sizeof(data), reg);
}

int wrmsr_on_cpu(uint32_t reg, int cpu, uint64_t data)
{
    char msr_file_name[64];
    
    sprintf(msr_file_name, "/dev/cpu/%d/msr", cpu);
    return file_write_offset(msr_file_name, (uint8_t*) &data, sizeof(data), reg);
}

uint64_t read_flags()
{
    uint64_t flags = 0x0;

    asm("pushfq\n\t"
        "popq %0\n\t"
        :"=m"(flags)::);

    return flags;
}

void clflush(void *p)
{
    asm volatile ("clflush 0(%0)\n"
      :
      : "c" (p)
      : "rax");
}
