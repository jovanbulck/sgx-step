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

#ifndef SGX_STEP_ENCLAVE_H
#define SGX_STEP_ENCLAVE_H
#if !NO_SGX

#include <stdint.h>

struct sgx_step_enclave_info
{
    uint64_t base;
    uint64_t limit;
    uint64_t exec_base;
    uint64_t exec_limit;
    uint64_t size;
    uint64_t aep;
    uint64_t tcs;
    char *drv;
};

extern uint64_t nemesis_tsc_eresume, nemesis_tsc_aex;
extern int sgx_step_eresume_cnt;
extern int sgx_step_do_trap;

typedef void (*aep_cb_t)(void);
void register_aep_cb(aep_cb_t cb);

void register_enclave_info(void);
void print_enclave_info(void);
void *get_enclave_base(void);
void *get_enclave_limit(void);
char *get_enclave_drv(void);
int get_enclave_size(void);
int get_enclave_exec_range(uint64_t *start, uint64_t *end);
int edbgrdwr(void *adrs, void* res, int len, int write);
#define edbgrd(adrs, res, len)  edbgrdwr(adrs, res, len, 0)
#define edbgwr(adrs, res, len)  edbgrdwr(adrs, res, len, 1)

void mark_enclave_exec_not_accessed(void);
uint64_t is_enclave_exec_accessed(void);
void dump_enclave_exec_pages(void);

/* NOTE: incorrect GPRSGX size in Intel manual vol. 3D June 2016 p.38-7 */
#define SGX_TCS_FLAGS_OFFSET        8
#define SGX_TCS_OSSA_OFFSET         16
#define SGX_TCS_CSSA_OFFSET         24
#define SGX_GPRSGX_SIZE             184
#define SGX_GPRSGX_RIP_OFFSET       136
#define SGX_FLAGS_DBGOPTIN          0x1

/* HACK: to avoid having to retrieve the SSA framesize from the untrusted
   runtime (driver), we assume a standard/hard-coded SSA framesize of 1 page */
//TODO determine this at runtime..
#ifndef SGX_SSAFRAMESIZE
    #define SGX_SSAFRAMESIZE            4096
#endif

struct gprsgx_region {
    uint64_t rax;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rbx;
    uint64_t rsp;
    uint64_t rbp;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t rflags;
    uint64_t rip;
    uint64_t ursp;
    uint64_t urbp;
    uint32_t exitinfo;
    uint32_t reserved;
    uint64_t fsbase;
    uint64_t gsbase;
};

typedef union {
    struct gprsgx_region fields;
    uint8_t bytes[ sizeof(struct gprsgx_region) ];
} gprsgx_region_t;

typedef union {
    uint8_t bytes[8];
    uint64_t reg;
} gpr_t;

void* get_enclave_ssa_gprsgx_adrs(void);
void dump_gprsgx_region(gprsgx_region_t *gprsgx_region);

uint64_t edbgrd_ssa_gprsgx(int gprsgx_field_offset);
#define edbgrd_erip() edbgrd_ssa_gprsgx(SGX_GPRSGX_RIP_OFFSET)

void set_debug_optin(void);

#endif
#endif
