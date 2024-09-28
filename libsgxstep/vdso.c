#include <sys/auxv.h>
#include <stdio.h>
#include <gelf.h>
#include <string.h>
#include <stdlib.h>
#include "debug.h"
#include <sys/mman.h>
#include <sys/ptrace.h>

void xs_trap_vdso(void)
{
    Elf_Scn *scn = NULL;
    char *vdso_base = (void *) getauxval(AT_SYSINFO_EHDR);
    ASSERT( vdso_base );
    ASSERT(elf_version(EV_CURRENT) != EV_NONE);

    info("found VDSO base at %p", vdso_base);

    GElf_Ehdr *ehdr = (GElf_Ehdr *) vdso_base;
    ASSERT( memcmp(ehdr->e_ident, ELFMAG, SELFMAG) == 0 && "ELF magic");

    /* Parse the ELF program header table to find the dynamic linking table */
    GElf_Phdr *phdrtab = (GElf_Phdr *) (vdso_base + ehdr->e_phoff);
    GElf_Dyn *dyntab = 0;
    uint64_t load_offset = 0;
    for (int i = 0; i < ehdr->e_phnum; i++)
    {
	if (phdrtab[i].p_type == PT_DYNAMIC)
	    dyntab = (GElf_Dyn *) (vdso_base + phdrtab[i].p_offset);
    }
    ASSERT( dyntab != 0 && "dyntab not found" );

    /* Parse the dynamic linking table to find the vdso symtab */
    GElf_Sym *symtab = NULL;
    char *strtab = NULL;
    for (int i = 0; dyntab[i].d_tag != DT_NULL; i++)
        if (dyntab[i].d_tag == DT_SYMTAB)
                symtab = (GElf_Sym *) (vdso_base + dyntab[i].d_un.d_ptr);
        else if (dyntab[i].d_tag == DT_STRTAB)
                strtab = vdso_base + dyntab[i].d_un.d_ptr;
    ASSERT( symtab && strtab ); 

    /* print symbol names */
	char *vdso_enter_pt = NULL;
    // index 0 is reserved: https://docs.oracle.com/cd/E23824_01/html/819-0690/chapter6-79797.html#chapter6-tbl-23
    for (int i = 1; ; i++) {
        // Assuming the symbol table is terminated by a symbol with st_value == 0
        if (symtab[i].st_value == 0) {
            break; // End of symbol table
        }
		if (!strcmp("__vdso_sgx_enter_enclave", strtab + symtab[i].st_name))
		{
			vdso_enter_pt = vdso_base + symtab[i].st_value;
			info("found `__vdso_sgx_enter_enclave` at %p", vdso_enter_pt);
		}
    }
}