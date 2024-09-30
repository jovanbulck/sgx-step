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
#include "sched.h"
#include "cpu.h"
#include "config.h"
#include <stdint.h>

/* APIC MMIO/MSR register address map (cf. SDM Table 11.6) */
#if !X2APIC
    #if APIC_CONFIG_MSR
        #define APIC_BASE_MSR_X2APIC    0x400
        #define APIC_BASE_MSR_ENABLE    0x800
        #define APIC_BASE_ADDR_MASK     0xfff
    #else
        #define APIC_BASE               0xfee00000
    #endif
    
    #define APIC_ID                     0x20
    #define APIC_EOI                    0xb0
    #define APIC_ICR_LOW                0x300
    #define APIC_ICR_HIGH               0x310
    #define APIC_LVTT                   0x320
    #define APIC_TMICT                  0x380
    #define APIC_TDCR                   0x3e0
#else /* X2APIC */
    #define APIC_ID                     0x802
    #define APIC_EOI                    0x80b
    #define APIC_ICR                    0x830
    #define APIC_LVTT                   0x832
    #define APIC_TMICT                  0x838
    #define APIC_TDCR                   0x83e
#endif /* X2APIC */

#define APIC_ID_SHIFT                   24
#define APIC_ID_MASK                    (0xff << APIC_ID_SHIFT)
#define APIC_TDR_DIV_1                  0xb
#define APIC_TDR_DIV_2                  0x0
#define APIC_LVTT_ONESHOT               (0 << 17)
#define APIC_LVTT_DEADLINE              (2 << 17)

#define APIC_ICR_VECTOR(n)              (n & 0xFF)
#define APIC_ICR_DELIVERY_FIXED         (0x0 << 8)
#define APIC_ICR_LEVEL_ASSERT           (0x1 << 14)
#define APIC_ICR_DEST_SELF              (0x1 << 18)
#define APIC_ICR_DEST_PHYSICAL          (0x0 << 11)
#define APIC_ICR_DEST_LOGICAL           (0x1 << 11)
#define APIC_ICR_DEST_MASK              0xff000000

extern int g_apic_setup;
extern uint64_t g_apic_deadline_tsc_begin;
void apic_init(void);
    
/* read/write functions for XAPIC in MMIO access mode */
#if !X2APIC
extern void* apic_base;
    /*
     * From Linux kernel source: /arch/x86/include/asm/apic.h
     * NOTE: Intel SDM: "any access that touches bytes 4 through 15 of an APIC
     * register may cause undefined behavior and must not be executed."
     */
    static inline int apic_write(uint32_t reg, uint32_t v)
    {
        volatile uint32_t *addr;
        if (!g_apic_setup) apic_init();
    
        addr = (volatile uint32_t *)(apic_base + reg);
        __asm__ volatile ("movl %1, %0\n\t"
            :"=m"(*addr):"r"(v):);
    
        return 0;
    }
    
    static inline uint32_t apic_read(uint32_t reg)
    {
        if (!g_apic_setup) apic_init();
    
        return *((volatile uint32_t *)(apic_base + reg));
    }
#else /* X2APIC */
    static inline int apic_write(uint32_t reg, uint32_t v)
    {
        if (!g_apic_setup) apic_init();
    
        wrmsr(reg, v);
        return 0;
    }

    static inline uint32_t apic_read(uint32_t reg)
    {
        if (!g_apic_setup) apic_init();
    
        return rdmsr(reg);
    }
#endif /* X2APIC */

#define apic_timer_irq(interval) apic_write(APIC_TMICT, interval);
#define apic_send_ipi_self(n)    apic_write(APIC_ICR_LOW, APIC_ICR_VECTOR(n) | APIC_ICR_DELIVERY_FIXED | APIC_ICR_LEVEL_ASSERT | APIC_ICR_DEST_SELF)

uint8_t apic_id(void);
int apic_timer_oneshot(uint8_t vector);
int apic_timer_deadline(uint8_t vector);
void apic_timer_deadline_irq(int tsc_offset);

#endif
