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

#include "libsgxstep/idt.h"
#include "libsgxstep/gdt.h"
#include "libsgxstep/apic.h"
#include "libsgxstep/cpu.h"
#include "libsgxstep/sched.h"
#include "libsgxstep/config.h"
#include <unistd.h>

#define DO_APIC_SW_IRQ              1
#define DO_APIC_TMR_IRQ             0
#define DO_EXEC_PRIV                0
#define NUM                         1000
#define NEMESIS_HIGH                1

/* ------------------------------------------------------------ */
/* This code may execute with ring0 privileges */
int my_cpl = -1;
extern uint64_t nemesis_tsc_aex, nemesis_tsc_eresume;

void pre_irq(void)
{
    my_cpl = get_cpl();
    __ss_irq_fired = 0;
    nemesis_tsc_eresume = rdtsc_begin();
}

void do_irq_sw(void)
{
    asm("int %0\n\t" ::"i"(IRQ_VECTOR):);
}


/* ------------------------------------------------------------ */

void post_irq(void)
{
    ASSERT(__ss_irq_fired);
    info("returned from IRQ: my_cpl=%d; irq_cpl=%d; count=%02d; flags=%p; nemesis=%d",
            my_cpl, __ss_irq_cpl, __ss_irq_count, read_flags(), nemesis_tsc_aex - nemesis_tsc_eresume);
}

int main( int argc, char **argv )
{
    if (argc == 2) {
        info("triggering sw IRQ on vector %d!", IRQ_VECTOR);
        do_irq_sw();
        return 0;
    }

    idt_t idt = {0};
    ASSERT( !claim_cpu(VICTIM_CPU) );
    map_idt(&idt);

    info_event("Installing and testing ring0 IDT handler");
    install_kernel_irq_handler(&idt, __ss_irq_handler, IRQ_VECTOR);

    info("wating for sw IRQ on vector %d ...", IRQ_VECTOR);

    pre_irq();

    while (__ss_irq_fired == 0) {
        sleep(1);
    }

    post_irq();

    info("all is well; irq_count=%d; exiting..", __ss_irq_count);
    return 0;

}
