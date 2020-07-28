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

#define DO_APIC_TMR_IRQ             1
#define DO_APIC_SW_IRQ              0
#define DO_USER_HANDLER             0
#define DO_USER_LOOP                0

void __ss_irq_handler(void);
extern int volatile __ss_irq_fired, __ss_irq_count, __ss_irq_cpl;

/* ------------------------------------------------------------ */
/* This code may execute with ring0 privileges */
int my_cpl = -1;

void do_irq_loop(void)
{
    my_cpl = get_cpl();
    __ss_irq_fired = 0;
    apic_timer_irq(10);
    while(!__ss_irq_fired);
}

void my_ring0_func(void)
{
    my_cpl = get_cpl();
}
/* ------------------------------------------------------------ */

int main( int argc, char **argv )
{
    idt_t idt = {0};
    ASSERT( !claim_cpu(VICTIM_CPU) );

    info_event("Establishing user space IDT mapping");
    map_idt(&idt);
    #if DO_USER_HANDLER
        install_user_asm_irq_handler(&idt, __ss_irq_handler, IRQ_VECTOR);
    #else
        install_kernel_irq_handler(&idt, __ss_irq_handler, IRQ_VECTOR);
    #endif
    //dump_idt(&idt);

    #if !DO_USER_LOOP
        exec_priv(my_ring0_func);
        info("back from my_ring0_func w CPL=%d", my_cpl);
    #endif

    #if DO_APIC_SW_IRQ
        info_event("Triggering user space software interrupts");
        asm("int %0\n\t" ::"i"(IRQ_VECTOR):);
        asm("int %0\n\t" ::"i"(IRQ_VECTOR):);
    #endif

    #if DO_APIC_TMR_IRQ
        info_event("Triggering user space APIC timer interrupts");
        apic_timer_oneshot(IRQ_VECTOR);

        for (int i=0; i < 3; i++)
        {
            //apic_send_ipi_self(IRQ_VECTOR);
            #if DO_USER_LOOP
                do_irq_loop();
            #else
                exec_priv(do_irq_loop);
            #endif
            info("returned from timer IRQ: my_cpl=%d; irq_cpl=%d; count=%d; flags=%p", my_cpl, __ss_irq_cpl, __ss_irq_count, read_flags());
        }

        apic_timer_deadline();
    #endif

    info("all is well; irq_count=%d; exiting..", __ss_irq_count);
    return 0;
}
