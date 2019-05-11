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
