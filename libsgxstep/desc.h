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

#ifndef SGX_STEP_DESC_H
#define SGX_STEP_DESC_H

#include <stdint.h>

/* IA-64: 16-byte gate (from Linux kernel arch/x86/include/asm/desc_defs.h) */
typedef struct {
    uint16_t offset_low;
    uint16_t segment;
    unsigned ist : 3, zero0 : 5, type : 5, dpl : 2, p : 1;
    uint16_t offset_middle;
    uint32_t offset_high;
    uint32_t zero1;
} __attribute__((packed)) gate_desc_t;

typedef enum {
    GATE_INTERRUPT = 0xE,
    GATE_TRAP = 0xF,
    GATE_CALL = 0xC,
    GATE_TASK = 0x5,
} gate_type_t;

typedef struct {
    uint32_t offset;
    uint16_t segment;
} __attribute__((packed)) call_gate_pt_t;

#define PTR_LOW(x) ((unsigned long long)(x) & 0xFFFF)
#define PTR_MIDDLE(x) (((unsigned long long)(x) >> 16) & 0xFFFF)
#define PTR_HIGH(x) ((unsigned long long)(x) >> 32)

#define gate_offset(g) ((g)->offset_low | ((unsigned long)(g)->offset_middle << 16) | ((unsigned long)(g)->offset_high << 32))
#define gate_ptr(base, idx) ((gate_desc_t*) (((void*) base) + idx*sizeof(gate_desc_t)))

/*
 * From Linux kernel arch/x86/include/asm/segment.h 
 *                   arch/x86/include/asm/desc_defs.h
 */
#define KERNEL_DPL          0
#define USER_DPL            3
#define GDT_ENTRY_USER_CS   6
#define GDT_ENTRY_KERNEL_CS 2

typedef enum {
    KERNEL_CS = GDT_ENTRY_KERNEL_CS*8+KERNEL_DPL,
    USER_CS   = GDT_ENTRY_USER_CS*8+USER_DPL,
} cs_t;

typedef struct {
    uint16_t size;
    uint64_t base;
} __attribute__((packed)) dtr_t;

#define dump_dtr(dtr, entries)                          \
    libsgxstep_info("DTR.base=%p/size=%d (%d entries)", \
        (void*) (dtr)->base, (dtr)->size, entries);

#endif
