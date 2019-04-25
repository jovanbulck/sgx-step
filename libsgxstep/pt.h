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

#ifndef SGX_STEP_MMAP_H
#define SGX_STEP_MMAP_H

#include <stdint.h>
#include <inttypes.h>
#include "../kernel/sgxstep_ioctl.h"

// Returns a mask of the form:
// +----- n+1 -+- n --------- 0-+
// | 0  0  0   |  1  1  1  1  1 |
// +-----------+----------------+
#define MASK_TO(m)		((UINT64_C(0x1) << ((m) + 1)) - 1 )

// Returns a mask of the form:
// +----- m+1 -+- m ------ n -+--- 0-+
// | 0  0  0   |  1  1  1  1  | 0  0 |
// +-----------+--------------+------+
// The ordered version requires n < m, the other CREATE_MASK checks this at runtime
#define CREATE_MASK_ORDERED(n,m)	(MASK_TO((m)) ^ (MASK_TO((n)) >> 1))
#define CREATE_MASK(n,m)	( ((n) < (m)) ? (CREATE_MASK_ORDERED((n), (m))) : (CREATE_MASK_ORDERED((m), (n))) )

#ifdef REDEFINE_M
#define M			REDEFINE_M
#else
#define M			(physical_address_width())
#endif

#define MASK_M			((uint64_t) ((INT64_C(0x1) << physical_address_width()) - 1))

#define A_SHIFT			5
#define A_MASK			(UINT64_C(0x1) << A_SHIFT)

#define D_SHIFT			6
#define D_MASK			(UINT64_C(0x1) << D_SHIFT)

#define W_SHIFT			1
#define W_MASK			(UINT64_C(0x1) << W_SHIFT)

#define US_SHIFT		2
#define US_MASK			(UINT64_C(0x1) << US_SHIFT)

#define XD_SHIFT		63
#define XD_MASK			(UINT64_C(0x1) << XD_SHIFT)

#define P_SHIFT			0
#define P_MASK			UINT64_C(0x1)

#define RSVD_SHIFT      51
#define RSVD_MASK       (UINT64_C(0x1) << RSVD_SHIFT)

#define PUD_PS_SHIFT		7
#define PUD_PS_MASK		(UINT64_C(0x1) << PUD_PS_SHIFT)

#define PMD_PS_SHIFT		7
#define PMD_PS_MASK		(UINT64_C(0x1) << PMD_PS_SHIFT)

#define PT_PHYS_SHIFT		12
#define PT_PHYS_MASK		(MASK_M << PT_PHYS_SHIFT)

#define PGD_SHIFT		39
#define PGD_MASK		(INT64_C(0x1ff) << PGD_SHIFT)

#define PUD_SHIFT		30
#define PUD_MASK		(INT64_C(0x1ff) << PUD_SHIFT)

#define PMD_SHIFT		21
#define PMD_MASK		(INT64_C(0x1ff) << PMD_SHIFT)

#define PT_SHIFT		12
#define PT_MASK			(INT64_C(0x1ff) << PT_SHIFT)

#define PAGE_SHIFT		0
#define PAGE_MASK		(INT64_C(0xfff) << PAGE_SHIFT)

#define PAGE1GiB_SHIFT		0
#define PAGE1GiB_MASK		(INT64_C(0x3FFFFFFF) << PAGE1GiB_SHIFT)

#define PAGE2MiB_SHIFT		0
#define PAGE2MiB_MASK		(INT64_C(0x1FFFFF) << PAGE2MiB_SHIFT)

#define PAGE_SIZE_4KiB		0x1000
#define PAGE_SIZE_2MiB		0x200000
#define PAGE_SIZE_1GiB		0x40000000

#define PFN_MASK                0xfffULL

#define ACCESSED(entry) 		(((entry) & A_MASK) >> A_SHIFT)
#define MARK_ACCESSED(entry) 		((entry) | A_MASK)
#define MARK_NOT_ACCESSED(entry) 	((entry) & ~A_MASK)

#define DIRTY(entry) 			(((entry) & D_MASK) >> D_SHIFT)
#define MARK_DIRTY(entry) 		((entry) | D_MASK)
#define MARK_CLEAN(entry) 		((entry) & ~D_MASK)

#define WRITABLE(entry) 		(((entry) & W_MASK) >> W_SHIFT)
#define MARK_WRITABLE(entry) 		((entry) | W_MASK)
#define MARK_NON_WRITABLE(entry)	((entry) & ~W_MASK)

#define USER(entry) 		    (((entry) & US_MASK) >> US_SHIFT)
#define MARK_USER(entry) 		((entry) | US_MASK)
#define MARK_SUPERVISOR(entry)	((entry) & ~US_MASK)

#define EXECUTE_DISABLE(entry)		((((entry) & XD_MASK) >> XD_SHIFT))
#define MARK_EXECUTE_DISABLE(entry)	((entry) | XD_MASK)
#define MARK_NOT_EXECUTE_DISABLE(entry)	((entry) & ~XD_MASK)

#define EXECUTABLE(entry) 		((EXECUTE_DISABLE(entry)) ? UINT64_C( 0 ) : UINT64_C( 1 ) )
#define MARK_EXECUTABLE(entry) 		MARK_NOT_EXECUTE_DISABLE(entry)
#define MARK_NON_EXECUTABLE(entry)	MARK_EXECUTE_DISABLE(entry)

#define PRESENT(entry)			(((entry) & P_MASK) >> P_SHIFT)
#define MARK_PRESENT(entry) 		((entry) | P_MASK)
#define MARK_NOT_PRESENT(entry) 	((entry) & ~P_MASK)

#define RSVD(entry)			    (((entry) & RSVD_MASK) >> RSVD_SHIFT)
#define MARK_RSVD(entry)        ((entry) | RSVD_MASK)
#define CLEAR_RSVD(entry)       ((entry) & ~RSVD_MASK)

#define PUD_PS(entry)			(((entry) & PUD_PS_MASK) >> PUD_PS_SHIFT)
#define PMD_PS(entry)			(((entry) & PMD_PS_MASK) >> PMD_PS_SHIFT)

#define PGD_PHYS(entry)			((entry) & (CREATE_MASK(12, M-1)))
#define PUD_PS_0_PHYS(entry)    ((entry) & (CREATE_MASK(12, M-1)))
#define PUD_PS_1_PHYS(entry)	((entry) & (CREATE_MASK(30, M-1)))
#define PMD_PS_0_PHYS(entry)	((entry) & (CREATE_MASK(12, M-1)))
#define PMD_PS_1_PHYS(entry)	((entry) & (CREATE_MASK(21, M-1)))
#define PT_PHYS(entry)			((entry) & (CREATE_MASK(12, M-1)))

#define PGD_INDEX(virt)			(virt & PGD_MASK) >> PGD_SHIFT
#define PUD_INDEX(virt)			(virt & PUD_MASK) >> PUD_SHIFT
#define PMD_INDEX(virt)			(virt & PMD_MASK) >> PMD_SHIFT
#define PT_INDEX(virt)			(virt & PT_MASK) >> PT_SHIFT
#define PAGE_INDEX(virt)		(virt & PAGE_MASK) >> PAGE_SHIFT
#define PAGE1GiB_INDEX(virt)	(virt & PAGE1GiB_MASK) >> PAGE1GiB_SHIFT
#define PAGE2MiB_INDEX(virt)	(virt & PAGE2MiB_MASK) >> PAGE2MiB_SHIFT

void mem_open( void );
void step_open( void );
void __attribute__((destructor))  tear_down_sgx_step( void );

typedef enum {PGD, PUD, PMD, PTE, PAGE} pt_level_t;

void *remap(uint64_t phys);
void free_map(void *p);
void *remap_page_table_level( void *address, pt_level_t level );
void flush_tlb(void *adrs);

address_mapping_t *get_mappings( void *address );
uint64_t phys_address( address_mapping_t *map, pt_level_t level );
uint64_t phys_base_address( address_mapping_t *map, pt_level_t level );
uint64_t virt_index( address_mapping_t *map, pt_level_t level );
uint64_t physical_address_width( void );

void print_page_table( void *address );
void print_pte_adrs( void *adrs);
void print_pte( uint64_t *pte );
void print_mapping( address_mapping_t *map );

#endif
