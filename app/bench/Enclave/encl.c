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
#include <string.h>

// see asm.S
extern char secret_str;
extern void zigzag_bench(uint64_t nb);

// see asm_nop.S
extern void asm_microbenchmark(void);
extern int my_strlen(const char *str);

void do_nop_slide(void) { asm_microbenchmark(); }

int a, b;

void do_zigzagger(int n) {
    a = 1;
    b = 0;
    zigzag_bench(n);
}

int do_strlen(int n) {
    int i, j;

    for (i = 0; i < n; i++) j = my_strlen(&secret_str);

    return j;
}
