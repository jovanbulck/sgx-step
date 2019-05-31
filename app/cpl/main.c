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

int gate_cpl = -1;
uint64_t gate_msr = -1;
void call_gate_func(void);

int main( int argc, char **argv )
{
    gdt_t gdt = {0};

    ASSERT( !claim_cpu(VICTIM_CPU) );

    info_event("Establishing user space GDT mapping");
    map_gdt(&gdt);
    dump_gdt(&gdt);

    info_event("Installing ring0 call gate");
    install_call_gate(&gdt, GDT_VECTOR, KERNEL_CS, call_gate_func);
    dump_gate(get_gate_desc(&gdt, GDT_VECTOR), GDT_VECTOR);
    do_far_call(GDT_VECTOR);

    info("back from call gate w CPL prev/cur=%d/%d; RDMSR=%p",
        gate_cpl, get_cpl(), gate_msr);
    return 0;
}
