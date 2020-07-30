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

#define DO_APIC_SW_IRQ              1
#define DO_APIC_TMR_IRQ             1
#define DO_EXEC_PRIV                1

/* ------------------------------------------------------------ */
/* This code may execute with ring0 privileges */
int my_cpl = -1;

void pre_irq(void)
{
    my_cpl = get_cpl();
    __ss_irq_fired = 0;
}

void do_irq_sw(void)
{
    pre_irq();
    asm("int %0\n\t" ::"i"(IRQ_VECTOR):);
}

void do_irq_tmr(void)
{
    pre_irq();
    apic_timer_irq(10);
    while(!__ss_irq_fired);
}

/* ------------------------------------------------------------ */

void post_irq(void)
{
    ASSERT(__ss_irq_fired);
    info("returned from IRQ: my_cpl=%d; irq_cpl=%d; count=%02d; flags=%p",
            my_cpl, __ss_irq_cpl, __ss_irq_count, read_flags());
}

void do_irq_test(int do_exec_priv)
{
    #if DO_APIC_SW_IRQ
        printf("\n");
        info("Triggering ring3 software interrupts..");
        for (int i=0; i < 3; i++)
        {
            do_irq_sw();
            post_irq();
        }

        if (do_exec_priv)
        {
            printf("\n");
            info("Triggering ring0 software interrupts..");
            for (int i=0; i < 3; i++)
            {
                exec_priv(do_irq_sw);
                post_irq();
            }
        }
    #endif

    #if DO_APIC_TMR_IRQ
        printf("\n");
        info("Triggering ring3 APIC timer interrupts..");
        apic_timer_oneshot(IRQ_VECTOR);

        for (int i=0; i < 3; i++)
        {
            do_irq_tmr();
            post_irq();
        }

        if (do_exec_priv)
        {
            printf("\n");
            info("Triggering ring0 APIC timer interrupts..");
            for (int i=0; i < 3; i++)
            {
                exec_priv(do_irq_tmr);
                post_irq();
            }
        }

        apic_timer_deadline();
    #endif
}

int main( int argc, char **argv )
{
    idt_t idt = {0};
    ASSERT( !claim_cpu(VICTIM_CPU) );

    info_event("Installing and testing ring3 IDT handler");
    map_idt(&idt);
    install_user_irq_handler(&idt, __ss_irq_handler, IRQ_VECTOR);
    do_irq_test(/*do_exec_priv=*/ 0);

    info_event("Installing and testing ring0 IDT handler");
    install_kernel_irq_handler(&idt, __ss_irq_handler, IRQ_VECTOR);
    #if DO_EXEC_PRIV
        exec_priv(pre_irq);
        info("back from exec_priv(pre_irq) with CPL=%d", my_cpl);
    #endif
    do_irq_test(/*do_exec_priv=*/ DO_EXEC_PRIV);

    info("all is well; irq_count=%d; exiting..", __ss_irq_count);
    return 0;
}
