#include "gdt.h"
#include "pt.h"

void dump_desc(desc_t *desc, int idx)
{
    info("GDT[%3d] @%p=0x%08x / 0x%05x (seg sel 0x%02x); p=%d; dpl=%d; type=%2d; g=%d",
        idx, desc, (unsigned int) desc_base(desc), (int) desc_limit(desc),
        idx*8+desc->dpl, desc->p, desc->dpl, desc->type, desc->g);
}

void dump_gdt(gdt_t *gdt)
{
    int i;

    info("--------------------------------------------------------------------------------");
    for (i =0; i < gdt->entries; i++)
        dump_desc(desc_ptr(gdt->base, i), i);
    info("--------------------------------------------------------------------------------");
}

void map_gdt(gdt_t *gdt)
{
    dtr_t gdtr = {0};
    void *gdt_base = NULL;
    int entries;

    asm volatile ("sgdt %0\n\t"
                  :"=m"(gdtr) :: );
    entries = (gdtr.size+1)/sizeof(desc_t);
    dump_dtr(&gdtr, entries);

    ASSERT( gdtr.base );
    gdt_base = remap_page_table_level((void*) gdtr.base, PAGE);
    libsgxstep_info("established user space GDT mapping at %p", gdt_base);
    ASSERT(gdt_base);

    gdt->base = (desc_t*) gdt_base;
    gdt->entries = entries;
}

desc_t *get_desc(gdt_t *gdt, int idx)
{
    ASSERT(idx >= 0 && idx < gdt->entries);

    return desc_ptr(gdt->base, idx);
}

gate_desc_t *get_gate_desc(gdt_t *gdt, int idx)
{
    /* System descriptors are expanded to 16 bytes (occupying the space of two entries). */
    ASSERT(idx >= 0 && idx < (gdt->entries-1));

    return (gate_desc_t*) (desc_ptr(gdt->base, idx));
}

int get_cpl(void)
{
    int rv;
    asm("mov %%cs, %0\n\t"
        "and $0x3, %0\n\t"
        :"=r"(rv)::);
    return rv;
}

/* NOTE: make sure SMAP/SMEP are disabled when installing ring0 user space call gates */
void install_call_gate(gdt_t *gdt, int gdt_idx, cs_t cs, call_gate_cb_t handler)
{
    ASSERT(gdt_idx >= 0 && gdt_idx < (gdt->entries-1));
    ASSERT( !get_desc(gdt, gdt_idx)->p && !get_desc(gdt, gdt_idx+1)->p );

    gate_desc_t *g = get_gate_desc(gdt, gdt_idx);
    g->offset_low    = PTR_LOW(handler);
    g->offset_middle = PTR_MIDDLE(handler);
    g->offset_high   = PTR_HIGH(handler);
    g->p             = 1;
    g->segment       = cs;
    g->dpl           = USER_DPL;
    g->type          = GATE_CALL;
    g->ist           = 0;
}

void do_far_call(int gdt_idx)
{
    call_gate_pt_t p = {
        .segment = (gdt_idx*8+USER_DPL)
    };
    asm("lcall *%0\n\t"::"m"(p):);
}
