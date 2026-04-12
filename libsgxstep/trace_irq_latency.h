#ifndef TRACE_IRQ_LATENCY_H
#define TRACE_IRQ_LATENCY_H

#include "trace_module.h"
#include "debug.h"
#include "pt.h"
#include "cache.h"
#include "enclave.h"
#include <string.h>
#include <sys/mman.h>

/* Entry in ds-bookeeper */
typedef struct{

} irq_entry_t;

typedef struct{

    uint32_t *bitmap_events;
    size_t time_step;                

} irq_module_state_t;

/*
    NOTE: These must be the same in all X_trace.h files
    used by the tracer.
*/
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
trace_module_t* trace_irq_create(void);
#endif