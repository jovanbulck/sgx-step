#ifndef SGX_TRACER
#define SGX_TRACER

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "trace_module.h"
#include "trace_pages.h"
#include "trace_gprs.h"
#include "trace_irq_latency.h"

#include "debug.h"
#include <string.h>
#include <time.h> // time in vcd

#define MAX_MODULES 4
#define MAX_STEPS 200000

/* ============ Future add to enum here :) ============== */
typedef enum{
    PAGES = 0,
    GPRS = 1,
    IRQ = 2,
} track_type_t;

typedef trace_module_t* (*create_module_fn)(void);
/* ======================= END ============================== */

typedef struct trace_module trace_module_t;
typedef struct{
    trace_module_t *module; // Has interface functions
    void *state;            // Pointer to opaque storage of events
} tracer_entry_t;

typedef struct{
    tracer_entry_t modules[MAX_MODULES];
    size_t num_modules;
    size_t step_count;
}sgx_tracer_t;

void sgx_tracer_man_add(sgx_tracer_t *t, track_type_t opt, void *items, size_t num_of_items);
void sgx_tracer_init(sgx_tracer_t *t);
void sgx_tracer_trace_all(sgx_tracer_t *t, const track_type_t *options, size_t opt_cnt);

void sgx_tracer_update(sgx_tracer_t *t);
void sgx_tracer_destroy(sgx_tracer_t *t);
void sgx_tracer_vcd(sgx_tracer_t *t);

#endif