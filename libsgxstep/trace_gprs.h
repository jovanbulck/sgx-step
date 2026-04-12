#ifndef TRACE_GPRS_H
#define TRACE_GPRS_H

#include "trace_module.h"
#include "debug.h"
#include "pt.h"
#include "cache.h"
#include "enclave.h"
#include <string.h>

#define SGX_GPRSGX_RIP_OFFSET       136
#define ALL_GPRS 24

// Enum of byte offsets
enum gprsgx_offset {
    RAX      = 0,
    RCX      = 8,
    RDX      = 16,
    RBX      = 24,
    RSP      = 32,
    RBP      = 40,
    RSI      = 48,
    RDI      = 56,
    R8       = 64,
    R9       = 72,
    R10      = 80,
    R11      = 88,
    R12      = 96,
    R13      = 104,
    R14      = 112,
    R15      = 120,
    RFLAGS   = 128,
    RIP      = 136,
    URSP     = 144,
    URBP     = 152,
    EXITINFO = 160,
    RESERVED = 164,
    FSBASE   = 168,
    GSBASE   = 176
};

static enum gprsgx_offset gpr_all[] = {
    RAX, RCX, RDX, RBX, RSP, RBP, RSI, RDI,
    R8, R9, R10, R11, R12, R13, R14, R15,
    RFLAGS, RIP, URSP, URBP, EXITINFO,
    RESERVED, FSBASE, GSBASE
};

static const char *gpr_names[] = {
    [RAX] = "RAX",
    [RCX] = "RCX",
    [RDX] = "RDX",
    [RBX] = "RBX",
    [RSP] = "RSP",
    [RBP] = "RBP",
    [RSI] = "RSI",
    [RDI] = "RDI",
    [R8] = "R8",
    [R9] = "R9",
    [R10] = "R10",
    [R11] = "R11",
    [R12] = "R12",
    [R13] = "R13",
    [R14] = "R14",
    [R15] = "R15",
    [RFLAGS] = "RFLAGS",
    [RIP] = "RIP",
    [URSP] = "URSP",
    [URBP] = "URBP",
    [EXITINFO] = "EXITINFO",
    [RESERVED] = "RESERVED",
    [FSBASE] = "FSBASE",
    [GSBASE] = "GSBASE"
};

typedef struct {
    enum gprsgx_offset sgx_gpr_off;
} gpr_entry_t;

typedef struct {

    //gpr_entry_t *tracked_gprs;
    enum gprsgx_offset *tracked_gprs;
    size_t num_tracked_gprs;
    size_t *bitmap_events;
    size_t internal_step;

} gprs_module_state_t;

/* ================ API ==================== */
static void* init(void);
static void* man_init(void *items, size_t num_of_items);
static void update(void *state);
static size_t count(void *state);   
static int get(void *state, size_t step, trace_signal_t *sig);
static void destroy(void *state);
static int describe(void *state, size_t index, char *name);
/* ======================================== */

/* Constructor for this module - registered in sgx_tracer.c factory */
trace_module_t* trace_gprs_create(void);

#endif