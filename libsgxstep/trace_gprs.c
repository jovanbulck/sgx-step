#include "trace_gprs.h"

/* Constructor for this module - registered in sgx_tracer.c factory */
trace_module_t* trace_gprs_create(void)
{
    trace_module_t *m = malloc(sizeof(*m));
    ASSERT( m != NULL );

    m->module_name = "gprs_trace";
    m->init     = init;
    m->man_init = man_init;
    m->update   = update;
    m->count    = count;
    m->get      = get;
    m->describe = describe;
    m->destroy  = destroy;

    return m;
}

static void* init(void)
{
    gprs_module_state_t *state = malloc(sizeof(gprs_module_state_t));
    ASSERT( state != NULL );


    state->num_tracked_gprs = ALL_GPRS;
    //state->tracked_gprs = gpr_all;      // no malloc in .rodata
					
    state->tracked_gprs = malloc(sizeof(gpr_all));
    ASSERT( state->tracked_gprs != NULL );
    memcpy(state->tracked_gprs, gpr_all, sizeof(gpr_all));

    /* Bitmap allocation (use size_t to be gpr size agnostic)*/
    //size_t words = (state->num_tracked_gprs) * sizeof(size_t);
    state->bitmap_events = calloc( (state->num_tracked_gprs) * MAX_STEPS_PER_MODULE, sizeof(size_t) );
    ASSERT( state->bitmap_events != NULL );

    state->internal_step = 0;
    return state;
}

static void* man_init(void *items, size_t num_of_items)
{
    ASSERT( num_of_items <= 24 );

    gprs_module_state_t *state = malloc(sizeof(gprs_module_state_t));
    ASSERT( state != NULL );

    state->num_tracked_gprs = num_of_items;

    enum gprsgx_offset *regs = (enum gprsgx_offset*) items;

    /* Allocate memory for these */
    //state->tracked_gprs = malloc(sizeof(gpr_entry_t) * num_of_items);
    state->tracked_gprs = malloc(sizeof(enum gprsgx_offset) * num_of_items);
    ASSERT( state->tracked_gprs != NULL );

    for (size_t i = 0; i < num_of_items; i++)
    {
        //state->tracked_gprs[i].sgx_gpr_off = regs[i];
        state->tracked_gprs[i] = regs[i];
    }

    /* Bitmap allocation (use size_t to be gpr size agnostic)*/
    state->bitmap_events = calloc( (state->num_tracked_gprs) * MAX_STEPS_PER_MODULE, sizeof(size_t) );
    ASSERT( state->bitmap_events != NULL );

    state->internal_step = 0;

    return state;
}

static void update(void *state)
{
    gprs_module_state_t *s = (gprs_module_state_t *) state;
    size_t *bitmap_ev = s->bitmap_events;
    enum gprsgx_offset *regs_off = s->tracked_gprs;

    (s->internal_step)++;
    for (size_t i = 0; i < s->num_tracked_gprs; i++)
    {
        bitmap_ev[i + (s->internal_step - 1) * s->num_tracked_gprs] = edbgrd_ssa_gprsgx(regs_off[i]);
    }
}

static void destroy(void *state)
{
    gprs_module_state_t *s = (gprs_module_state_t *) state;
    free(s->tracked_gprs);
    free(s->bitmap_events);
}

static size_t count(void *state)
{
    gprs_module_state_t *s = (gprs_module_state_t *) state;
    return s->num_tracked_gprs;
}

static int get(void *state, size_t step, trace_signal_t *sig)
{
    gprs_module_state_t *s = (gprs_module_state_t *) state;
    ASSERT( step < s->internal_step && step < MAX_STEPS_PER_MODULE);

    sig->items = s->num_tracked_gprs; 
    sig->bits_per_item = sizeof(size_t) * __CHAR_BIT__;

    /* payload at specific step */
    sig->payload = &s->bitmap_events[step * s->num_tracked_gprs];

    return 0;
}

static int describe(void *state, size_t index, char *name)
{
    gprs_module_state_t *s = state;

    if (index >= s->num_tracked_gprs)
        return -1;

    enum gprsgx_offset reg = s->tracked_gprs[index];

    strcpy(name, gpr_names[reg]);

    return 0;
}