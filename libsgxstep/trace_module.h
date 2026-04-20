#ifndef TRACE_MODULE_H
#define TRACE_MODULE_H

#include <stddef.h>
#include "debug.h"

typedef struct trace_module trace_module_t;
typedef struct trace_signal trace_signal_t;

#define MAX_STEPS_PER_MODULE 200000

typedef struct trace_module trace_module_t;
struct trace_module {

    /* module internal state */
    void *state;

    /* Module name for tracking*/
    char *module_name;

    /* WHat sgx_tracer uses to tell module what to do */
    //void* (*init)(void);
    void (*init)(trace_module_t *m);

    //void* (*mod_init)(void *items, size_t num_of_items);
    void* (*opt_add)(trace_module_t *m, void *opt, size_t opt_len);

    //void (*step)(void *state);
    void (*step)(trace_module_t *m);

    //void (*destroy)(void *state);
    void (*destroy)(trace_module_t *m);

    /* name of signal */
    //int (*describe)(void *state, size_t index, char *name); /* what is tracked at index? */
    int (*describe)(trace_module_t *m , size_t index, char *name); /* what is tracked at index? */

    /* no. of items we are tracking in module */
    //size_t (*count)(void *state);
    size_t (*count)(trace_module_t *m);

    /* payload at a certain step */
    //int (*get)(void *state, size_t step, trace_signal_t *sig);
    int (*get)(trace_module_t *m , size_t step, trace_signal_t *sig);
};

struct trace_signal{
    void *payload;          // pointer to opaque ds at that step
    size_t items;           // How many items in entry.
    size_t bits_per_item;    
};
#endif