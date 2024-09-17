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

typedef struct {
    gate_desc_t *base;
    size_t     entries;
} idt_t;

typedef void (*irq_cb_t)(uint8_t *rsp);
typedef void (*exec_priv_cb_t)(void);

/*
 * Executes the provided callback function with ring-0 privileges by installing
 * a custom interrupt gate.
 *
 * NOTE: Calling `exec_priv` may lead to unpredictable system freezes when
 * passing larger or complex functions. Keep in mind the following for the
 * callback function:
 *
 *      1. Executes with interrupts disabled, best to keep it short.
 *      2. Don't use any system calls (e.g., libc).
 *      3. Avoid page-fault exceptions: no illegal accesses and preferably
 *      `mlock` all code/data pages.
 *
 * While `exec_priv` may be greatly useful to quickly test out some privileged
 * functionality in ring-0 C code without recompiling the kernel, if long-term
 * stability is desired it may be best to pass a carefully crafted asm callback
 * function.
 */
void exec_priv(exec_priv_cb_t cb);
void trigger_sw_irq();

void map_idt(idt_t *idt);
void dump_idt(idt_t *idt);
void dump_gate(gate_desc_t *gate, int idx);

void install_user_irq_handler(idt_t *idt, void* asm_handler, int vector);
void install_kernel_irq_handler(idt_t *idt, void *asm_handler, int vector);
void install_priv_gate(void *asm_handler, int vector);

void __ss_irq_handler(void);
extern int volatile __ss_irq_fired, __ss_irq_count, __ss_irq_cpl;
extern uint64_t __ss_irq_rip;
extern uint64_t nemesis_tsc_aex;

#endif
