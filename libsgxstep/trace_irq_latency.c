#include "trace_irq_latency.h"

/* 
    NOTE: See irq_entry.S to see aex rtsc read there and
          See aep_trampoline.S to see eresume tsc. 
          For nemesis aex __ss_irq_handler must be called 
          (i.e. registered as the isr for the apic irq)
          Thus, you need production enclave to run and not step with RFLAGS
*/
extern uint64_t nemesis_tsc_eresume, nemesis_tsc_aex;

/* Constructor for this module - registered in sgx_tracer.c factory */
trace_module_t* trace_irq_create(void)
{
    trace_module_t *m = malloc(sizeof(*m));
    ASSERT( m != NULL );

    m->module_name  = "irq_latency_trace";
    m->init         = init;
    m->man_init     = man_init;
    m->update       = update;
    m->count        = count;
    m->get          = get;
    m->describe     = describe;
    m->destroy      = destroy;

    return m;
}

static void* init(void)
{
    irq_module_state_t *state = malloc(sizeof(irq_module_state_t));
    ASSERT( state != NULL );

    state->bitmap_events = calloc(MAX_STEPS_PER_MODULE, sizeof(uint32_t));
    state->time_step = 0;
    ASSERT( state->bitmap_events != NULL );
    return state;
}

static void* man_init(void *items, size_t num_of_items)
{
    return init();
}

static void update(void *state)
{
    irq_module_state_t *s = (irq_module_state_t *) state;

    uint32_t *bitmap_ev = s->bitmap_events;

    (s->time_step)++;
    bitmap_ev[(s->time_step - 1)] = (uint32_t)(nemesis_tsc_aex - nemesis_tsc_eresume);
}

static void destroy(void *state)
{
    irq_module_state_t *s = (irq_module_state_t *) state;
    free(s->bitmap_events);
    free(s);
}

static size_t count(void *state)
{
    return 1; /* only one irq number :) */
}

static int get(void *state, size_t step, trace_signal_t *sig)
{
    irq_module_state_t *s = (irq_module_state_t *) state;
    ASSERT( step < s->time_step );

    sig->items = 1;
    sig->bits_per_item = 32;

    /* payload at specifc step */
    sig->payload = &s->bitmap_events[step];

    return 0;
}

static int describe(void *state, size_t index, char *name)
{
    strcpy(name, "IRQ_LATENCY");
    return 0;
}