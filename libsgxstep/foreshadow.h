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
#ifndef LIBSGXSTEP_FORESHADOW_H_INC
#define LIBSGXSTEP_FORESHADOW_H_INC

#include <inttypes.h>
#include "enclave.h"

/*
 * NOTE: As speculative reading tend to have a slight bias towards zero values; we
 * retry a few times before concluding the secret byte is indeed zero.
 */
#define FORESHADOW_ZERO_RETRIES     20

/* Be extra suspicious of zero bytes (which might mean the SSA cacheline got kicked,
 * and we need to reload it via a fresh faulting ERESUME call) */
#define FORESHADOW_SSA_ZERO_RETRIES 20

#define FORESHADOW_DEBUG            0

#define CEIL(x,y)			        ( ((x) % (y) == 0 ) ? ((x) / (y)) : ((x) / (y)) + 1 )

#define NEGATIVE_MARGIN_SLOTS		5
#define POSITIVE_MARGIN_SLOTS		5
#define NUM_SLOTS			        256
#define SLOT_SIZE			        (64*64)
#define SLOT_OFFSET(base, index)	((uint8_t *)((uint64_t) (base) + (index) * SLOT_SIZE))
#define ORACLE_POSITIVE_MARGIN_SIZE	(POSITIVE_MARGIN_SLOTS * SLOT_SIZE)
#define ORACLE_NEGATIVE_MARGIN_SIZE	(NEGATIVE_MARGIN_SLOTS * SLOT_SIZE)
#define ORACLE_MAIN_BUFFER_SIZE		(NUM_SLOTS * SLOT_SIZE)
#define TOTAL_ORACLE_PAGES		    ( CEIL( ORACLE_POSITIVE_MARGIN_SIZE, 0x1000 ) + CEIL( ORACLE_MAIN_BUFFER_SIZE, 0x1000 ) + CEIL( ORACLE_NEGATIVE_MARGIN_SIZE, 0x1000 ) )

uint8_t *create_oracle_buffer( void );
void destroy_oracle_buffer( uint8_t *oracle_buffer );

int foreshadow(void *adrs, void *oracle);
int foreshadow_ssa(gprsgx_region_t *shadow_gprsgx, void* gprsgx_alias, void* oracle);

void foreshadow_dump_perf(void);
int foreshadow_compare_secret(uint8_t *recovered, uint8_t *real, int len);

#endif
