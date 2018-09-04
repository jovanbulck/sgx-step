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

#ifndef SGX_STEP_DESC_H
#define SGX_STEP_DESC_H

#include <stdint.h>

/*
 * From Linux kernel arch/x86/include/asm/segment.h 
 *                   arch/x86/include/asm/desc_defs.h
 */
#define KERNEL_DPL          0
#define USER_DPL            3
#define GDT_ENTRY_USER_CS   6
#define USER_CS             (GDT_ENTRY_USER_CS*8+USER_DPL)
#define GDT_ENTRY_KERNEL_CS 2
#define KERNEL_CS           (GDT_ENTRY_KERNEL_CS*8+KERNEL_DPL)

typedef struct {
    uint16_t size;
    uint64_t base;
} __attribute__((packed)) dtr_t;

#define dump_dtr(dtr, entries)                      \
    info("DTR.base=%p/size=%d (%d entries)",       \
        (void*) (dtr)->base, (dtr)->size, entries);

#endif
