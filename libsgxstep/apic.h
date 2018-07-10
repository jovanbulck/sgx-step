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

#ifndef SGX_STEP_APIC_H
#define SGX_STEP_APIC_H

#include "debug.h"
#include "config.h"
#include <stdint.h>

#if APIC_CONFIG_MSR
    #define APIC_BASE_MSR_X2APIC    0x400
    #define APIC_BASE_MSR_ENABLE    0x800
    #define APIC_BASE_ADDR_MASK     0xfff
#else
    #define APIC_BASE               0xfee00000
#endif

#define APIC_ICR                    0x300
#define APIC_LVTT                   0x320
#define APIC_TDCR                   0x3e0
#define APIC_TMICT                  0x380
#define APIC_TMCCT                  0x390

#define APIC_ID                     0x20

#define APIC_TDR_DIV_1              0xb
#define APIC_TDR_DIV_2              0x0
#define APIC_LVTT_ONESHOT           (0 << 17)
#define APIC_LVTT_DEADLINE          (2 << 17)

#define LOCAL_TIMER_VECTOR          0xef
#define APIC_IPI_CFG                0xc08f1

extern void* apic_base;
void apic_init(void);
void apic_hook(void);

/*
 * From Linux kernel source: /arch/x86/include/asm/apic.h
 * NOTE: Intel SDM: "any access that touches bytes 4 through 15 of an APIC
 * register may cause undefined behavior and must not be executed."
 */
static inline int apic_write(uint32_t reg, uint32_t v)
{
    volatile uint32_t *addr;
    if (!apic_base) apic_init();

    addr = (volatile uint32_t *)(apic_base + reg);
    asm volatile ("movl %1, %0\n\t"
        :"=m"(*addr):"r"(v):);

    return 0;
}

static inline uint32_t apic_read(uint32_t reg)
{
    if (!apic_base) apic_init();

    return *((volatile uint32_t *)(apic_base + reg));
}

#define apic_send_ipi() apic_write(APIC_ICR, APIC_IPI_CFG)
#define apic_timer_irq(tsc) apic_write(APIC_TMICT, tsc);

int apic_timer_oneshot(void);
int apic_timer_deadline(void);

#endif
