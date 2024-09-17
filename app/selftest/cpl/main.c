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

#include "libsgxstep/gdt.h"
#include "libsgxstep/idt.h"
#include "libsgxstep/cpu.h"
#include "libsgxstep/sched.h"
#include "libsgxstep/config.h"

#define USE_CALL_GATE           1
#define USE_IRQ_GATE            1
#define IA32_TIME_STAMP_COUNTER 0x10

int gate_cpl = -1;
uint64_t gate_msr = -1;
uint64_t my_msr = -1;
void call_gate_func(void);
void irq_gate_func(void);
uint64_t do_rdmsr(void);

int my_cpl = -1;

/* ------------------------------------------------------------ */
/* This code will execute with ring0 privileges :) */
void my_ring0_func(void)
{
    my_cpl = get_cpl();
    my_msr = do_rdmsr();
}
/* ------------------------------------------------------------ */

int main( int argc, char **argv )
{
    ASSERT( !claim_cpu(VICTIM_CPU) );

#if USE_CALL_GATE
    gdt_t gdt = {0};
    info_event("Establishing user space GDT mapping");
    map_gdt(&gdt);
    dump_gdt(&gdt);

    info_event("Installing and calling ring0 call gate");
    install_call_gate(&gdt, GDT_VECTOR, KERNEL_CS, call_gate_func);
    dump_gate(get_gate_desc(&gdt, GDT_VECTOR), GDT_VECTOR);

    gate_msr = gate_cpl = -1;
    do_far_call(GDT_VECTOR);

    info("back from call gate w CPL prev/cur=%d/%d; RDMSR=%p",
        gate_cpl, get_cpl(), gate_msr);
#endif

#if USE_IRQ_GATE
    idt_t idt = {0};
    info_event("Establishing user space IDT mapping");
    map_idt(&idt);
    //dump_idt(&idt);

    info_event("Installing and calling ring0 irq gate");
    install_kernel_irq_handler(&idt, irq_gate_func, IRQ_VECTOR);

    gate_msr = gate_cpl = -1;
    asm("int %0\n\t" ::"i"(IRQ_VECTOR):);

    info("back from irq gate w CPL prev/cur=%d/%d; RDMSR=%p",
        gate_cpl, get_cpl(), gate_msr);

    info_event("Calling ring0 function on user page with `exec_priv()`");
    exec_priv(my_ring0_func);
    info("back from my_ring0_func w CPL prev/cur=%d/%d; RDMSR=%p",
        my_cpl, get_cpl(), my_msr);
#endif

    return 0;
}
