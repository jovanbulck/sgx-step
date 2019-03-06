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
#ifndef LIBSGXSTEP_CACHE_H_INC
#define LIBSGXSTEP_CACHE_H_INC

/* 
 * Code adapted from: Yarom, Yuval, and Katrina Falkner. "Flush+ reload: a high
 * resolution, low noise, L3 cache side-channel attack." 23rd USENIX Security
 * Symposium (USENIX Security 14). 2014.
 */
inline unsigned long __attribute__((always_inline)) reload( void * adrs)
{
    volatile unsigned long time;

    asm volatile (
	"mfence\n\t"
	"lfence\n\t"
	"rdtsc\n\t"
	"lfence\n\t"
	"movl %%eax, %%esi\n\t"
	"movl (%1), %%eax\n\t"
	"lfence\n\t"
	"rdtsc\n\t"
	"subl %%esi, %%eax \n\t"
	: "=a" (time)
	: "c" (adrs)
	: "%rsi", "%rdx");

    return time;
}

inline void __attribute__((always_inline)) flush(void* p)
{
    asm volatile (	"mfence\n"
			"clflush 0(%0)\n"
			"mfence\n"
      :
      : "D" (p)
      : "rax");
}

inline void __attribute__((always_inline)) maccess(void* p)
{
    asm volatile (
    "mov (%0), %%rax\n"
    :
    : "c" (p)
    : "rax");
}

#endif
