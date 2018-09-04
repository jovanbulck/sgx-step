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

#ifndef SGX_STEP_AEP_H
#define SGX_STEP_AEP_H

#include <stdint.h>

extern uint32_t nemesis_tsc_eresume, nemesis_tsc_aex;

typedef void (*aep_cb_t)(void);
void register_aep_cb(aep_cb_t cb);

void register_enclave_info(void);
void print_enclave_info(void);
void *get_enclave_base(void);
int get_enclave_size(void);
void edbgrd(void *adrs, void* res, int len);

/* NOTE: incorrect GPRSGX size in Intel manual vol. 3D June 2016 p.38-7 */
#define SGX_TCS_OSSA_OFFSET         16
#define SGX_GPRSGX_SIZE             184
#define SGX_GPRSGX_RIP_OFFSET       136

/* HACK: to avoid having to retrieve the SSA framesize from the untrusted
   runtime (driver), we assume a standard/hard-coded SSA framesize of 1 page */
#define SGX_SSAFRAMESIZE            4096

uint64_t edbgrd_ssa(int ssa_field_offset);
#define edbgrd_erip() edbgrd_ssa(SGX_GPRSGX_RIP_OFFSET)

#endif
