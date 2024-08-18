/*
 *  This file is part of the SGX-Step enclave execution control framework.
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
#include <sgx_trts.h>
#include <stdint.h>
#include <string.h>

#define OFFSET 20
#define LFENCE 0

inline void __attribute__((always_inline)) maccess(void *p) {
    asm volatile("movb (%0), %%cl\n" : : "c"(p) :);
}

inline void __attribute__((always_inline)) flush(void *p) {
    asm volatile("clflush 0(%0)\n" : : "r"(p) :);
}

char __attribute__((aligned(0x1000))) dummy[4096 * 256];

void transient_delay(void) {
    /* delay to provide sufficient transient execution window */
    flush(dummy);
    maccess(dummy);
}

char __attribute__((aligned(0x1000))) page_a[4096] = {'A'};
char __attribute__((aligned(0x1000))) page_b[4096] = {'B'};
#define pt_a (page_a + OFFSET)
#define pt_b (page_b + OFFSET)

void ecall_lvi_store_user(uint64_t *user_pt, char *oracle) {
    if (sgx_is_outside_enclave(oracle, 4096 * 256) &&
        sgx_is_outside_enclave(user_pt, sizeof(uint64_t))) {
        /* 0. Fence to protect against Spectre v1 */
        __builtin_ia32_lfence();
        transient_delay();

        /* 1. STORE to attacker-controlled _untrusted_ address */
        *user_pt = (uint64_t)'S';

        /* 2. VICTIM LOAD: inject 'S' and override trusted value 'B' */
        volatile char valb = *pt_b;

#if LFENCE
        asm("lfence");
#endif

        /* 3. VICTIM ENCODE: e.g., cache-based covert channel gadget */
        volatile char leak = oracle[4096 * valb];
    }
}

void ecall_lvi_remap_l1d(char *oracle) {
    /* VICTIM PREFETCH: load 'A' at valid enclave physical address into L1 */
    volatile char vala = *pt_a;
    /* ensure 'A' is cached and load/store buffers are drained */
    asm("mfence");
    transient_delay();

    /* VICTIM LOAD: inject 'A' from remapped physical address for trusted load
     * to 'B'*/
    volatile char valb = *pt_b;

#if LFENCE
    asm("lfence");
#endif

    /* VICTIM ENCODE: e.g., cache-based covert channel gadget */
    volatile char leak = oracle[4096 * valb];
}
