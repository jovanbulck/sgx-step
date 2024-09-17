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
#define NUM                         10
#define INFINITE_LOOP               0
#define NEMESIS_HIGH                1

/* ------------------------------------------------------------ */
/* This code may execute with ring0 privileges */
int my_cpl = -1;
uint64_t my_flags = 0;
extern uint64_t nemesis_tsc_aex, nemesis_tsc_eresume;
int apic_oneshot = 1;

void pre_irq(void)
{
    my_cpl = get_cpl();
    my_flags = read_flags();
    __ss_irq_fired = 0;
    nemesis_tsc_eresume = rdtsc_begin();
}

void do_irq_sw(void)
{
    pre_irq();
    asm("int %0\n\t" ::"i"(IRQ_VECTOR):);
}

void do_irq_tmr(void)
{
    pre_irq();

    if (apic_oneshot)
    {
        apic_timer_irq(10);
    }
    else
    {
        apic_timer_deadline_irq(100);
    }

    /*
     * Ring-0 `exec_priv` handler executes with interrupts disabled FLAGS.IF=0
     * (as the kernel may freeze when interrupting a ring-0 trap gate), so the
     * APIC timer IRQ should be handled after returning.
     */
    if (!(my_flags & (0x1 << 9)))
        return;

    while (!__ss_irq_fired)
    {
        #if NEMESIS_HIGH
            asm("rdrand %rax\n\t");
        #else
            asm("nop\n\t");
        #endif
    }
}

/* ------------------------------------------------------------ */

void post_irq(char *s)
{
    ASSERT(__ss_irq_fired);
    info("returned from %s IRQ: my_cpl=%d; irq_cpl=%d; my_flags=%p; count=%02d; nemesis=%d", s,
            my_cpl, __ss_irq_cpl, my_flags, __ss_irq_count, nemesis_tsc_aex - nemesis_tsc_eresume);
}

void do_irq_apic_tmr_test(char *tmr_desc, int do_exec_priv)
{
    printf("\n");
    info_event("Triggering ring-3 %s interrupts..", tmr_desc);
    for (int i=0; i < NUM; i++)
    {
        do_irq_tmr();
        post_irq(tmr_desc);
    }

    if (do_exec_priv)
    {
        printf("\n");
        info_event("Triggering ring-0 %s interrupts..", tmr_desc);
        for (int i=0; i < NUM; i++)
        {
            my_cpl = -1;
            exec_priv(do_irq_tmr);
            while (!__ss_irq_fired);
            post_irq(tmr_desc);
        }
    }
}

void do_irq_test(int do_exec_priv)
{
    #if DO_APIC_SW_IRQ
        printf("\n");
        info_event("Triggering ring-3 software interrupts..");
        for (int i=0; i < NUM; i++)
        {
            do_irq_sw();
            post_irq("software");
        }

        if (do_exec_priv)
        {
            printf("\n");
            info_event("Triggering ring-0 software interrupts..");
            for (int i=0; i < NUM; i++)
            {
                my_cpl = -1;
                exec_priv(do_irq_sw);
                post_irq("software");
            }
        }
    #endif

    #if DO_APIC_TMR_IRQ
        apic_timer_oneshot(IRQ_VECTOR);
        apic_oneshot = 1;
        do_irq_apic_tmr_test("APIC timer oneshot", do_exec_priv);

        apic_timer_deadline(IRQ_VECTOR);
        apic_oneshot = 0;
        do_irq_apic_tmr_test("APIC timer tsc_deadline", do_exec_priv);
    #endif
}

int main( int argc, char **argv )
{
    idt_t idt = {0};
    ASSERT( !claim_cpu(VICTIM_CPU) );
    map_idt(&idt);

    #if 0
    /* ring 3 timer IRQ handlers may #GP when interrupting the kernel.. */
    info_event("Installing and testing ring3 IDT handler");
    install_user_irq_handler(&idt, __ss_irq_handler, IRQ_VECTOR);
    do_irq_test(/*do_exec_priv=*/ 0);
    #endif

    info_event("Installing and testing ring-0 IDT handler");
    install_kernel_irq_handler(&idt, __ss_irq_handler, IRQ_VECTOR);

    #if DO_EXEC_PRIV
        exec_priv(pre_irq);
        info("back from exec_priv(pre_irq) with CPL=%d", my_cpl);
    #endif

    #if INFINITE_LOOP
    while(1)
    #endif
        do_irq_test(/*do_exec_priv=*/ DO_EXEC_PRIV);

    info("all is well; irq_count=%d; exiting..", __ss_irq_count);
    return 0;
}
