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

#ifndef SGX_STEP_IDT_H
#define SGX_STEP_IDT_H

#include "desc.h"
#include "debug.h"

/* IA-64: 16-byte gate (from Linux kernel arch/x86/include/asm/desc_defs.h) */
typedef struct {
    uint16_t offset_low;
    uint16_t segment;
    unsigned ist : 3, zero0 : 5, type : 5, dpl : 2, p : 1;
    uint16_t offset_middle;
    uint32_t offset_high;
    uint32_t zero1;
} __attribute__((packed)) idt_gate_t;

enum {
    GATE_INTERRUPT = 0xE,
    GATE_TRAP = 0xF,
    GATE_CALL = 0xC,
    GATE_TASK = 0x5,
};

#define PTR_LOW(x) ((unsigned long long)(x) & 0xFFFF)
#define PTR_MIDDLE(x) (((unsigned long long)(x) >> 16) & 0xFFFF)
#define PTR_HIGH(x) ((unsigned long long)(x) >> 32)

#define gate_offset(g) ((g)->offset_low | ((unsigned long)(g)->offset_middle << 16) | ((unsigned long)(g)->offset_high << 32))
#define gate_ptr(idt_base, idx) ((idt_gate_t*) (((void*) idt_base) + idx*sizeof(idt_gate_t)))

typedef struct {
    idt_gate_t *base;
    size_t     entries;
} idt_t;

typedef void (*irq_cb_t)(uint8_t *rsp);

void map_idt(idt_t *idt);
void dump_idt(idt_t *idt);
void dump_gate(idt_gate_t *gate, int idx);

void install_user_irq_handler(idt_t *idt, irq_cb_t handler, int vector);

#endif
