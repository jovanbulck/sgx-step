#include "trace_module.h"
#include "debug.h"
#include "pt.h"
#include "cache.h"
#include "enclave.h"
#include <sys/mman.h>


/* Entry in ds-bookeeper */
typedef struct{
    uint64_t *pte;
    void *page_base;
} page_entry_t;

/* State of page tracker */
typedef struct{

    page_entry_t *tracked_pages;      // Trakced pages (ds A)
    size_t num_tracked_pages;        

    uint64_t *bitmap_events;
    size_t time_step;                

} page_module_state_t;

#define BITS 64

#define WORDS(X) ((X + (BITS - 1)) / BITS);

/* ================== Internal ======================= */
static void mark_not_accessed(page_entry_t *tracked_pages, size_t num_tracked_pages);
void page_dbg_log(void *state);
/* ==================================================== */

static void opt_add(trace_module_t *m, void *opt, size_t opt_len)
{
    void *base = get_enclave_base();
    void *limit = get_enclave_limit();

    /* Allocate state */
    page_module_state_t *state = malloc(sizeof(page_module_state_t));
    ASSERT( state != NULL );

    state->num_tracked_pages = opt_len;

    void **pages = (void **)opt; // convert to an array of pointers/addresses

    /* Allocate pages to track */
    state->tracked_pages = malloc(sizeof(page_entry_t) * opt_len);
    ASSERT( state->tracked_pages != NULL );

    for (size_t i = 0; i < opt_len; i++) 
    {
        ASSERT( base <= pages[i] && limit > pages[i] );
	
	/* Fault-in the page*/
        uint8_t v = *(uint8_t *)pages[i];
        uint64_t *pte = remap_page_table_level(pages[i], PTE);
        ASSERT( pte );
        ASSERT( PRESENT(*pte) );
        state->tracked_pages[i] = (page_entry_t) { .page_base = pages[i], .pte = pte };
    }

    /* Bitmap meathod */
    size_t words = WORDS(state-> num_tracked_pages);
    state->bitmap_events = calloc(words * MAX_STEPS_PER_MODULE, sizeof(uint64_t) );
    state->time_step = 0;
    ASSERT( state->bitmap_events != NULL );

    m->state = state;
}

static void init(trace_module_t *m)
{
    size_t opt_pages = 0;

    void *base = get_enclave_base();
    void *limit = get_enclave_limit();

    /* find number of pages */
    int encl_pages = (get_enclave_size() + (PAGE_SIZE_4KiB - 1)) / PAGE_SIZE_4KiB;

    void **opt = malloc(sizeof(void*) * encl_pages);
    ASSERT( opt != NULL );
    
    ASSERT( (opt_pages = get_enclave_readable_pages(encl_pages, opt)) >  0 );

    /* pass _all_ pages to opt_add */
    opt_add(m, opt, opt_pages);
    
    info("Tracking %lu", opt_pages);
    free(opt);
}

static void step(trace_module_t *m)
{
    page_module_state_t *s = (page_module_state_t *) m->state;
    page_entry_t *pages = s->tracked_pages;
    uint64_t *bitmap_ev = s->bitmap_events;
    size_t words = WORDS(s->num_tracked_pages); /* How many cells a single bit map is*/

    (s->time_step)++;
    /* Check accessed bit of tracked pages*/
    for (size_t i = 0; i < s->num_tracked_pages; i++)
    {
        if ( ACCESSED( *(pages[i].pte)) )
        {
            bitmap_ev[ i / 64 + ((s->time_step - 1) * words)] |= ((uint64_t)1) << (i % 64);
        }
    }

    /* Clear access bit */
    mark_not_accessed(s->tracked_pages, s->num_tracked_pages);
}

static void destroy(trace_module_t *m)
{
    //page_dbg_log(state);
    page_module_state_t *s = (page_module_state_t *) m->state;

    free(s->tracked_pages);
    free(s->bitmap_events);
    free(s);
}

static size_t count(trace_module_t *m)
{
    page_module_state_t *s = (page_module_state_t *) m->state;
    return s->num_tracked_pages;
}

static int get(trace_module_t *m , size_t step, trace_signal_t *sig)
{
    page_module_state_t *s = (page_module_state_t *) m->state;
    ASSERT( step < s->time_step );

    size_t words = WORDS(s->num_tracked_pages);

    sig->items = s->num_tracked_pages;
    sig->bits_per_item = 1;

    /* payload at specifc step */
    sig->payload = &s->bitmap_events[step * words];

    return 0;
}

static int describe(trace_module_t *m, size_t index, char *name)
{
    void *base = get_enclave_base();

    page_module_state_t *s = (page_module_state_t *) m->state;

    ASSERT(index < s->num_tracked_pages);

    snprintf(name, 32, "%p", (void *) (s->tracked_pages[index].page_base - base));

    return 0;
}

/* =========================== Internal helpers =============================== */
/* Instead of clearing access bit in AEP call it automatically */
static void mark_not_accessed(page_entry_t *tracked_pages, size_t num_tracked_pages)
{
    for (size_t i = 0; i < num_tracked_pages; i ++)
    {
        uint64_t *pte = tracked_pages[i].pte;
        /* Swapped pages should be ignored*/
        if (!pte || !PRESENT(*pte))
            continue;

        *pte = MARK_NOT_ACCESSED( *pte );
        flush(pte);
    }
}

void page_dbg_log(void *state)
{
    page_module_state_t *s = (page_module_state_t *) state;

    FILE *f2 = fopen("pages_dbg_bit.txt", "w");
    if(!f2)
    {
        perror("(write_readable_log) fopen error\n");
        return;
    }

    uint64_t words = WORDS(s->num_tracked_pages);
    fprintf(f2, "# step,page\n");
    for (size_t step = 0; step < s->time_step; step++)
    {
        uint64_t *row = &s->bitmap_events[step * words];
    
        fprintf(f2, "%zu ", step);
    
        for (size_t j = 0; j < words; j++)
            fprintf(f2, "%" PRIu64 " ", row[j]);
    
        fprintf(f2, "\n");
    }

    fclose(f2);
}


trace_module_t* trace_pages_create(void)
{
    trace_module_t *m = malloc(sizeof(*m));
    ASSERT( m != NULL );

    m->module_name = "page_trace";
    m->init     = init;
    m->opt_add  = opt_add;
    m->step     = step;
    m->count    = count;
    m->get      = get;
    m->describe = describe;
    m->destroy  = destroy;

    return m;
}
