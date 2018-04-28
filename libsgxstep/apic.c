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
#include <fcntl.h>
#include <sys/ioctl.h>
#include "../kernel/sgxstep_ioctl.h"

extern int fd_step;
void *apic_base = NULL;
void *dummy_pt = NULL;

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
    apic_base = remap(APIC_BASE);
    info("established local APIC memory mapping at %p", apic_base);

    info("apic_id is %x", apic_read(APIC_ID));
    ASSERT(apic_read(APIC_ID) != -1);
}

void apic_hook(void)
{
    ASSERT((fd_step >= 0) && (ioctl(fd_step, SGX_STEP_IOCTL_LAPIC_HOOK, (uintptr_t)&dummy_pt) >= 0));
}

int apic_timer_oneshot(void)
{
    apic_write(APIC_LVTT, LOCAL_TIMER_VECTOR | APIC_LVTT_ONESHOT);
    apic_write(APIC_TDCR, APIC_TDR_DIV_2);
    // NOTE: APIC seems not to handle divide by 1 properly (?)
    // see also: http://wiki.osdev.org/APIC_timer)
    info("APIC timer one-shot mode with division 2 (lvtt=%x/tdcr=%x)",
        apic_read(APIC_LVTT), apic_read(APIC_TDCR));
}

int apic_timer_deadline(void)
{
    apic_write(APIC_LVTT, LOCAL_TIMER_VECTOR | APIC_LVTT_DEADLINE);
    apic_write(APIC_TDCR, APIC_TDR_DIV_2);
    info("APIC timer deadline mode with division 2 (lvtt=%x/tdcr=%x)",
        apic_read(APIC_LVTT), apic_read(APIC_TDCR));
}
