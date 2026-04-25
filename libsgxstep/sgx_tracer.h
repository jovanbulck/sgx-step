#ifndef SGX_TRACER
#define SGX_TRACER

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "trace_module.h"

#include "debug.h"

#include <string.h>
#include <time.h> // time in vcd

#define MAX_STEPS 200000

/* ============ Future add to enum here :) ============== */
typedef enum {
    TRACK_PAGES   = (1u << 0),
    TRACK_GPRS    = (1u << 1),
    TRACK_IRQ     = (1u << 2),
    TRACK_ALL     = TRACK_PAGES | TRACK_GPRS | TRACK_IRQ,
    MAX_MODULES   = __builtin_popcount(TRACK_ALL),
} track_type_t;
/* ======================= END ============================== */

typedef struct{
    trace_module_t *modules[MAX_MODULES];
    size_t num_modules;
    size_t step_count;
}sgx_tracer_t;

void sgx_tracer_init(sgx_tracer_t *t);
void sgx_tracer_init_mod(sgx_tracer_t *t, track_type_t opt, void *items, size_t num_of_items);
void sgx_tracer_add_module(sgx_tracer_t *t, const track_type_t modules);
void sgx_tracer_step(sgx_tracer_t *t);
void sgx_tracer_destroy(sgx_tracer_t *t);
void sgx_tracer_vcd(sgx_tracer_t *t);

#endif
