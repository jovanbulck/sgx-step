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

#include "apic.h"
#include "pt.h"
#include "cpu.h"
#include "sched.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include "../kernel/sgxstep_ioctl.h"

extern void *apic_base;
void *dummy_pt = NULL;
uint32_t apic_lvtt = 0x0, apic_tdcr = 0x0;

/*
 * Code below maps APIC timer MMIO registers in user space.
 *
 * NOTE: we require xAPIC mode, since "In x2APIC mode, the memory mapped
 * interface is not available and any access to the MMIO interface will behave
 * similar to that of a legacy xAPIC in globally disabled state" (Intel SDM
 * 10.12.2).
 *
 * Advised Linux kernel parameters are: "nox2apic iomem=relaxed no_timer_check"
 */
void apic_init(void)
{
    if (apic_base) return;

    uintptr_t apic_base_addr = 0x0;
    #if APIC_CONFIG_MSR
        uint64_t apic_base_msr = 0x0;
        rdmsr_on_cpu(IA32_APIC_BASE_MSR, get_cpu(), &apic_base_msr);
        ASSERT( (apic_base_msr & APIC_BASE_MSR_ENABLE) );
        ASSERT( !(apic_base_msr & APIC_BASE_MSR_X2APIC) );
        apic_base_addr = apic_base_msr & ~APIC_BASE_ADDR_MASK;
    #else
        apic_base_addr = APIC_BASE;
    #endif

    apic_base = remap(apic_base_addr);
    libsgxstep_info("established local memory mapping for APIC_BASE=%p at %p", (void*) apic_base_addr, apic_base);

    libsgxstep_info("APIC_ID=%x; LVTT=%x; TDCR=%x", apic_read(APIC_ID),
        apic_read(APIC_LVTT), apic_read(APIC_TDCR));
    ASSERT(apic_read(APIC_ID) != -1);
}

int apic_timer_oneshot(uint8_t vector)
{
    /* Save APIC tmr config for later restore */
    apic_lvtt = apic_read(APIC_LVTT);
    apic_tdcr = apic_read(APIC_TDCR);

    apic_write(APIC_LVTT, vector | APIC_LVTT_ONESHOT);
    apic_write(APIC_TDCR, APIC_TDR_DIV_2);
    // NOTE: APIC seems not to handle divide by 1 properly (?)
    // see also: http://wiki.osdev.org/APIC_timer)
    libsgxstep_info("APIC timer one-shot mode with division 2 (lvtt=%x/tdcr=%x)",
        apic_read(APIC_LVTT), apic_read(APIC_TDCR));
}

int apic_timer_deadline(void)
{
    if (apic_lvtt)
    {
        apic_write(APIC_LVTT, apic_lvtt);
        apic_write(APIC_TDCR, apic_tdcr);
        libsgxstep_info("Restored APIC_LVTT=%x/TDCR=%x)",
            apic_read(APIC_LVTT), apic_read(APIC_TDCR));
        apic_lvtt = apic_tdcr = 0x0;
    }

    /* writing a non-zero value to the TSC_DEADLINE MSR will arm the timer */
    /* In xAPIC mode the memory-mapped write to LVTT needs to be serialized. */
    #if APIC_CONFIG_MSR
        asm volatile("mfence" : : : "memory");
        wrmsr_on_cpu(IA32_TSC_DEADLINE_MSR, get_cpu(), 1);
    #endif
}
