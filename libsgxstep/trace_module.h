#ifndef TRACE_MODULE_H
#define TRACE_MODULE_H

#include <stddef.h>
#include "debug.h"

typedef struct trace_module trace_module_t;
typedef struct trace_signal trace_signal_t;

#define MAX_STEPS_PER_MODULE 200000

struct trace_module {

    /* Module name for tracking*/
    char *module_name;

    /* WHat sgx_tracer uses to tell module what to do */
    void* (*init)(void);
    void* (*man_init)(void *items, size_t num_of_items);
    void   (*update)(void *state);
    void   (*destroy)(void *state);

    /* name of signal */
    int (*describe)(void *state, size_t index, char *name); /* what is tracked at index? */
    /* items we are tracking in module */
    size_t (*count)(void *state);
    /* payload at a certain step */
    int (*get)(void *state, size_t step, trace_signal_t *sig);
};

struct trace_signal{
    void *payload;          // pointer to opaque ds at that step
    size_t items;           // How many items in entry.
    size_t bits_per_item;    
};

#endif