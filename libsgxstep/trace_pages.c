#include "trace_pages.h"

/* ================== Internal ======================= */
static void mark_not_accessed(page_entry_t *tracked_pages, size_t num_tracked_pages);
void page_dbg_log(void *state);
/* ==================================================== */

/* Constructor for this module - registered in sgx_tracer.c factory */
trace_module_t* trace_pages_create(void)
{
    trace_module_t *m = malloc(sizeof(*m));
    ASSERT( m != NULL );

    m->module_name = "page_trace";
    m->init     = init;
    m->man_init = man_init;
    m->update   = update;
    m->count    = count;
    m->get      = get;
    m->describe = describe;
    m->destroy  = destroy;

    return m;
}

static void* man_init(void *items, size_t num_of_items)
{
    void *base = get_enclave_base();
    void *limit = get_enclave_limit();

    /* Allocate state */
    page_module_state_t *state = malloc(sizeof(page_module_state_t));
    ASSERT( state != NULL );

    state->num_tracked_pages = num_of_items;

    void **pages = (void **)items; // convert to an array of pointers/addresses

    /* Allocate pages to track */
    state->tracked_pages = malloc(sizeof(page_entry_t) * num_of_items);
    ASSERT( state->tracked_pages != NULL );

    for (size_t i = 0; i < num_of_items; i++) 
    {
        ASSERT( base <= pages[i] && limit > pages[i] );
        uint64_t *pte = remap_page_table_level(pages[i], PTE);
        ASSERT( pte );
        state->tracked_pages[i] = (page_entry_t) { .page_base = pages[i], .pte = pte };
    }

    /* Bitmap meathod */
    size_t words =   (state->num_tracked_pages + 63) / 64; /* How many cells a single bit map is*/
    state->bitmap_events = calloc(words * MAX_STEPS_PER_MODULE, sizeof(uint64_t) );
    state->time_step = 0;
    ASSERT( state->bitmap_events != NULL );

    return state;

}

static void* init(void)
{
    /* Allocate state */
    page_module_state_t *state = malloc(sizeof(page_module_state_t));
    ASSERT( state != NULL );

    /* Find how many pages to track */
    void *base = get_enclave_base();
    void *limit = get_enclave_limit();
    state->num_tracked_pages = 0;

    for (void *p = base; p < limit; p += PAGE_SIZE_4KiB)
    {

        //uint64_t *pte = remap_page_table_level(p, PTE);
        //if (!pte || !PERSENT( *pte ) )
        //    continue;

        (state->num_tracked_pages)++;
    }

    /* Allocate pages to track */
    state->tracked_pages = malloc(sizeof(page_entry_t) * state->num_tracked_pages);
    ASSERT( state->tracked_pages != NULL );
    
    
    /* Get pages for the table */
    size_t i = 0;
    for (void *p = base; p < limit; p += PAGE_SIZE_4KiB)
    {
        uint64_t *pte = remap_page_table_level(p, PTE);
        ASSERT( pte );
        state->tracked_pages[i] = (page_entry_t) { .page_base = p, .pte = pte };
        i++;
    }

    info("All tracked pages: %zu", state->num_tracked_pages);

    /* Bitmap meathod */
    size_t words =   (state->num_tracked_pages + 63) / 64; /* How many cells a single bit map is*/
    state->bitmap_events = calloc(words * MAX_STEPS_PER_MODULE, sizeof(uint64_t) );
    state->time_step = 0;
    ASSERT( state->bitmap_events != NULL );

    return state;
}

static void update(void *state)
{
    page_module_state_t *s = (page_module_state_t *) state;
    page_entry_t *pages = s->tracked_pages;
    uint64_t *bitmap_ev = s->bitmap_events;
    size_t words = (s->num_tracked_pages + 63) / 64; /* How many cells a single bit map is*/

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

static void destroy(void *state)
{
    //page_dbg_log(state);
    page_module_state_t *s = (page_module_state_t *) state;

    free(s->tracked_pages);
    //free(s->events);
    free(s->bitmap_events);
    free(s);
}

static size_t count(void *state)
{
    page_module_state_t *s = (page_module_state_t *) state;
    return s->num_tracked_pages;
}

static int get(void *state, size_t step, trace_signal_t *sig)
{
    page_module_state_t *s = (page_module_state_t *) state;
    ASSERT( step < s->time_step );

    size_t words = (s->num_tracked_pages + 63) / 64;

    sig->items = s->num_tracked_pages;
    sig->bits_per_item = 1;

    /* payload at specifc step */
    sig->payload = &s->bitmap_events[step * words];

    return 0;
}

static int describe(void *state, size_t index, char *name)
{
    void *base = get_enclave_base();

    page_module_state_t *s = (page_module_state_t *) state;

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

    uint64_t words = (s->num_tracked_pages + 63) / 64;
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