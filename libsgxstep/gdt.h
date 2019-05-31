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

#ifndef SGX_STEP_GDT_H
#define SGX_STEP_GDT_H

#include "desc.h"
#include "debug.h"

/*
 * IA-64: normal (non LDT/TSS) GDT entries are still 8 bytes
 * (from Linux kernel arch/x86/include/asm/desc_defs.h)
 */
typedef struct {
    uint16_t limit0;
    uint16_t base0;
    unsigned base1: 8, type: 4, s: 1, dpl: 2, p: 1;
    unsigned limit1: 4, avl: 1, l: 1, d: 1, g: 1, base2: 8;
} __attribute__((packed)) desc_t;

#define desc_base(d) ((d)->base0 | ((unsigned long)(d)->base1 << 16) | ((unsigned long)(d)->base2 << 24))
#define desc_limit(d) ((d)->limit0 | ((unsigned long)(d)->limit1 << 16))
#define desc_ptr(gdt_base, idx) ((desc_t*) (((void*) gdt_base) + idx*sizeof(desc_t)))

typedef struct {
    desc_t  *base;
    size_t   entries;
} gdt_t;

typedef void (*call_gate_cb_t)(void);

void map_gdt(gdt_t *gdt);
void dump_gdt(gdt_t *gdt);
void dump_desc(desc_t *desc, int idx);
void install_call_gate(gdt_t *gdt, int gdt_idx, cs_t cs, call_gate_cb_t handler);
void do_far_call(int gdt_idx);

desc_t *get_desc(gdt_t *gdt, int idx);
gate_desc_t *get_gate_desc(gdt_t *gdt, int idx);

int get_cpl(void);

#endif
