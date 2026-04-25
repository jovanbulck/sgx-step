#include "trace_module.h"
#include "debug.h"
#include "pt.h"
#include "cache.h"
#include "enclave.h"
#include <string.h>


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

/* Use after entire region is read to map caps name to field */
static inline uint64_t gprsgx_get(const gprsgx_region_t *gprsgx_region, enum gprsgx_offset reg) 
{ 
    switch (reg) 
    { 
#define X(name, field, offset) case name: return gprsgx_region->fields.field; 
    GPRSGX_FIELDS(X) 
#undef X 
    } 

    return 0; 
}


static void init(trace_module_t *m)
{
    gprs_module_state_t *state = malloc(sizeof(gprs_module_state_t));
    ASSERT( state != NULL );

    state->num_tracked_gprs = ALL_GPRS;
    //state->tracked_gprs = gpr_all;      // no malloc in .rodata
					
    state->tracked_gprs = malloc(sizeof(gpr_all));
    ASSERT( state->tracked_gprs != NULL );
    memcpy(state->tracked_gprs, gpr_all, sizeof(gpr_all));

    /* Bitmap allocation (use size_t to be gpr size agnostic)*/
    state->bitmap_events = calloc( (state->num_tracked_gprs) * MAX_STEPS_PER_MODULE, sizeof(size_t) );
    ASSERT( state->bitmap_events != NULL );

    state->internal_step = 0;

    m->state = state;
}

static void opt_add(trace_module_t *m, void *opt, size_t opt_len)
{
    ASSERT( opt_len <= ALL_GPRS );

    gprs_module_state_t *state = malloc(sizeof(gprs_module_state_t));
    ASSERT( state != NULL );

    state->num_tracked_gprs = opt_len;

    enum gprsgx_offset *regs = (enum gprsgx_offset*) opt;

    /* Allocate memory for these */
    state->tracked_gprs = malloc(sizeof(enum gprsgx_offset) * opt_len);
    ASSERT( state->tracked_gprs != NULL );

    for (size_t i = 0; i < opt_len; i++)
    {
        state->tracked_gprs[i] = regs[i];
    }

    /* Bitmap allocation (use size_t to be gpr size agnostic)*/
    state->bitmap_events = calloc( (state->num_tracked_gprs) * MAX_STEPS_PER_MODULE, sizeof(size_t) );
    ASSERT( state->bitmap_events != NULL );

    state->internal_step = 0;

    m->state = state;
}

static void step(trace_module_t *m)
{

    /* Read entire gprsgx region */
    gprsgx_region_t gprsgx = {0};
    edbgrd(get_enclave_ssa_gprsgx_adrs(), &gprsgx, sizeof(gprsgx_region_t));
    //dump_gprsgx_region(&gprsgx);

    gprs_module_state_t *s = (gprs_module_state_t *) m->state;
    size_t *bitmap_ev = s->bitmap_events;
    enum gprsgx_offset *regs_off = s->tracked_gprs;

    (s->internal_step)++;
    for (size_t i = 0; i < s->num_tracked_gprs; i++)
    {
        bitmap_ev[i + (s->internal_step - 1) * s->num_tracked_gprs] = gprsgx_get(&gprsgx, regs_off[i]);
    }
}

static void destroy(trace_module_t *m)
{
    gprs_module_state_t *s = (gprs_module_state_t *) m->state;
    free(s->tracked_gprs);
    free(s->bitmap_events);
}

static size_t count(trace_module_t *m)
{
    gprs_module_state_t *s = (gprs_module_state_t *) m->state;
    return s->num_tracked_gprs;
}

static int get(trace_module_t *m, size_t step, trace_signal_t *sig)
{
    gprs_module_state_t *s = (gprs_module_state_t *) m->state;
    ASSERT( step < s->internal_step && step < MAX_STEPS_PER_MODULE);

    sig->items = s->num_tracked_gprs; 
    sig->bits_per_item = sizeof(size_t) * __CHAR_BIT__;

    /* payload at specific step */
    sig->payload = &s->bitmap_events[step * s->num_tracked_gprs];

    return 0;
}

static int describe(trace_module_t *m, size_t index, char *name)
{
    gprs_module_state_t *s = m->state;

    if (index >= s->num_tracked_gprs)
        return -1;

    enum gprsgx_offset reg = s->tracked_gprs[index];

    strcpy(name, gpr_names[reg]);

    return 0;
}

/* Constructor for this module - registered in sgx_tracer.c factory */
trace_module_t* trace_gprs_create(void)
{
    trace_module_t *m = malloc(sizeof(*m));
    ASSERT( m != NULL );

    m->module_name = "gprs_trace";
    m->init     = init;
    m->opt_add  = opt_add;
    m->step     = step;
    m->count    = count;
    m->get      = get;
    m->describe = describe;
    m->destroy  = destroy;

    return m;
}
