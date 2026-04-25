#include "trace_module.h"

#include "debug.h"
#include "pt.h"
#include "cache.h"
#include "enclave.h"
#include <string.h>
#include <sys/mman.h>

typedef struct{

    uint32_t *step_latencies;
    size_t current_step;                

} irq_module_state_t;

/* 
    NOTE: See irq_entry.S to see aex rtsc read there and
          See aep_trampoline.S to see eresume tsc. 
          For nemesis aex __ss_irq_handler must be called 
          (i.e. registered as the isr for the apic irq)
          Thus, you need production enclave to run and not step with RFLAGS
*/
extern uint64_t nemesis_tsc_eresume, nemesis_tsc_aex;


/* Version with internal state */
static void init(trace_module_t *m)
{
    irq_module_state_t *state = malloc(sizeof(irq_module_state_t));
    ASSERT( state != NULL );

    state->step_latencies = calloc(MAX_STEPS_PER_MODULE, sizeof(uint32_t));
    state->current_step = 0;
    ASSERT( state->step_latencies != NULL );

    m->state = state;
}

static void opt_add(trace_module_t *m, void *opt, size_t opt_len)
{
    init(m);
}

static void step(trace_module_t *m)
{
    irq_module_state_t *s = (irq_module_state_t *) m->state;

    uint32_t *bitmap_ev = s->step_latencies;

    (s->current_step)++;
    bitmap_ev[(s->current_step - 1)] = (uint32_t)(nemesis_tsc_aex - nemesis_tsc_eresume);
}

static void destroy(trace_module_t *m)
{
    irq_module_state_t *s = (irq_module_state_t *) m->state;
    free(s->step_latencies);
    free(s);
}

static size_t count(trace_module_t *m)
{
    return 1; /* only one irq number :) */
}

static int get(trace_module_t *m, size_t step, trace_signal_t *sig)
{
    irq_module_state_t *s = (irq_module_state_t *) m->state;
    ASSERT( step < s->current_step );

    sig->items = 1;
    sig->bits_per_item = 32;

    /* payload at specifc step */
    sig->payload = &s->step_latencies[step];

    return 0;
}

static int describe(trace_module_t *m, size_t index, char *name)
{
    strcpy(name, "IRQ_LATENCY");
    return 0;
}

trace_module_t* trace_irq_create(void)
{
    trace_module_t *m = malloc(sizeof(*m));
    ASSERT( m != NULL );

    m->module_name  = "irq_latency_trace";
    m->init         = init;
    m->opt_add      = opt_add;
    m->step         = step;
    m->count        = count;
    m->get          = get;
    m->describe     = describe;
    m->destroy      = destroy;

    return m;
}
