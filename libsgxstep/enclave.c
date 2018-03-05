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

#include "enclave.h"
#include "debug.h"
#include <sys/ioctl.h>
#include "../kernel/sgxstep_ioctl.h"

/* Includes custom AEP get/set functions from patched SGX SDK urts. */
#include <sgx_urts.h>

/* See aep_trampoline.S to see how these are used. */
extern void sgx_step_aep_trampoline(void);
aep_cb_t sgx_step_aep_cb = NULL;
uint64_t sgx_step_tcs    = 0x0;
uint64_t sgx_step_erip   = 0x0;

extern int fd_step;
struct sgx_step_enclave_info victim = {0};
int ioctl_init = 0;

void register_aep_cb(aep_cb_t cb)
{
    sgx_set_aep(sgx_step_aep_trampoline);
    sgx_step_aep_cb = cb;
}

void register_enclave_info(int edbgrd_rip)
{
    ASSERT(fd_step >= 0);
    sgx_set_aep(sgx_step_aep_trampoline);

    victim.aep = (uint64_t) sgx_get_aep();
    victim.tcs = (uint64_t) sgx_get_tcs();
    victim.erip_pt = edbgrd_rip ? (void*) &sgx_step_erip : NULL;
    ASSERT(ioctl(fd_step, SGX_STEP_IOCTL_VICTIM_INFO, &victim) >= 0);
    ioctl_init = 1;
}

void *get_enclave_base(void)
{
    ASSERT(ioctl_init && "/dev/sgx-step enclave_info struct not filled in");
    return (void*) victim.base;
}

int get_enclave_size(void)
{
    ASSERT(ioctl_init && "/dev/sgx-step enclave_info struct not filled in");
    return (int) victim.size;
}

void edbgrd(void *adrs, void* res, int len)
{
    edbgrd_t edbgrd_data = {
        .adrs = (uint64_t) adrs,
        .val = (uint8_t*) res,
        .len = len
    };

    ASSERT( ioctl(fd_step, SGX_STEP_IOCTL_EDBGRD, &edbgrd_data) >= 0 );
}

void print_enclave_info(void)
{
    uint64_t read = 0xff;

	printf( "==== Victim Enclave ====\n" );
	printf( "    Base: %p\n", get_enclave_base() );
	printf( "    Size: %d\n", get_enclave_size() );
    printf( "    Limit:  %p\n", get_enclave_base()+get_enclave_size() );
	printf( "    TCS:  %p\n", sgx_get_tcs() );
	printf( "    AEP:  %p\n", sgx_get_aep() );

    /* First 8 bytes of TCS must be zero */
    edbgrd( sgx_get_tcs(), &read, 8);
    printf( "    EDBGRD: %s\n", read ? "production" : "debug");
}
