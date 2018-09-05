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

#define USER_CS_CONFIRMING          0
#define DO_APIC_TMR_IRQ             1
#define DO_APIC_SW_IRQ              1

extern void my_irq_handler(void);
uint64_t irq_handler_count = 0x0;

int volatile irq_fired = 0, irq_count = 0;

void hello_world(uint8_t *rsp)
{
    uint64_t *p = (uint64_t*) rsp;
    printf("\n");
    info("****** hello world from user space IRQ handler with count=%d ******",
        irq_count++);

    info("APIC TPR/PPR is %d/%d", apic_read(APIC_TPR), apic_read(APIC_PPR));
    info("RSP at %p", rsp);
    info("RIP is %p", *p++);
    info("CS is %p", *p++);
    info("EFLAGS is %p", *p++);

    irq_fired = 1;
}

int main( int argc, char **argv )
{
    idt_t idt = {0};
    gdt_t gdt = {0};
    desc_t *user_cs = NULL;

    ASSERT( !claim_cpu(VICTIM_CPU) );

    info_event("Establishing user space GDT mapping");
    map_gdt(&gdt);
    #if USER_CS_CONFIRMING
        user_cs = get_desc(&gdt, GDT_ENTRY_USER_CS);
        user_cs->type = 15;
    #endif
    dump_gdt(&gdt);

    info_event("Establishing user space IDT mapping");
    map_idt(&idt);
    install_user_irq_handler(&idt, hello_world, IRQ_VECTOR);
    dump_idt(&idt);

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
            irq_fired = 0;
            apic_timer_irq(10);
            while(!irq_fired);
            info("returned from timer IRQ: flags=%p", read_flags());
        }

        apic_timer_deadline();
    #endif

    info("all is well; irq_handler_count=%d; exiting..", irq_count);
    return 0;
}
