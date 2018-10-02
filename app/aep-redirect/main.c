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

#include <sgx_urts.h>
#include "Enclave/encl_u.h"
#include <sys/mman.h>
#include <signal.h>
#include "libsgxstep/enclave.h"
#include "libsgxstep/debug.h"
#include "libsgxstep/pt.h"

void *a_pt;
int fault_fired = 0, aep_fired = 0;

void aep_cb_func(void)
{
    uint64_t erip = edbgrd_erip() - (uint64_t) get_enclave_base();
    info("Hello world from AEP callback with erip=%#llx! Resuming enclave..", erip); 

    aep_fired++;
}

void fault_handler(int signal)
{
	info("Caught fault %d! Restoring access rights..", signal);
    ASSERT(!mprotect(a_pt, 4096, PROT_READ | PROT_WRITE));
    print_pte_adrs(a_pt);
    fault_fired++;
}

int main( int argc, char **argv )
{
	sgx_launch_token_t token = {0};
	int updated = 0;
    sgx_enclave_id_t eid = 0;

   	info("Creating enclave...");
	SGX_ASSERT( sgx_create_enclave( "./Enclave/encl.so", /*debug=*/1,
                                    &token, &updated, &eid, NULL ) );
    register_aep_cb(aep_cb_func);
    register_enclave_info();
    print_enclave_info();

    /* mprotect to provoke page faults during enclaved execution */
    info("revoking a access rights..");
    SGX_ASSERT( get_a_addr(eid, &a_pt) );
    info("a at %p", a_pt);
    print_pte_adrs(a_pt);
    ASSERT(!mprotect(a_pt, 4096, PROT_NONE));
    print_pte_adrs(a_pt);
    ASSERT(signal(SIGSEGV, fault_handler) != SIG_ERR);

    info("calling enclave..");
    SGX_ASSERT( enclave_dummy_call(eid) );

    ASSERT(fault_fired && aep_fired);
   	SGX_ASSERT( sgx_destroy_enclave( eid ) );

    info("all is well; exiting..");
	return 0;
}
