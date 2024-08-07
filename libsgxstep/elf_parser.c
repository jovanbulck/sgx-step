#include "elf_parser.h"
#include "debug.h"
#include <fcntl.h>
#include <gelf.h>
#include <libelf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct symbol symbols[MAX_SYMBOLS] = {0};
int symbol_count = 0;

uintptr_t get_symbol_offset(const char *name) {
    if (!name) {
        return 0;
    }

    for (size_t i = 0; i < symbol_count; i++) {
        if (symbols[i].name != NULL && strcmp(symbols[i].name, name) == 0) {
            return symbols[i].address;
        }
    }

    return 0;
}

void add_symbol(const char *name, uintptr_t address) {
    ASSERT(symbol_count < MAX_SYMBOLS);
    ASSERT(name);
    symbols[symbol_count].name = strdup(name);
    symbols[symbol_count].address = address;
    symbol_count++;
}

void free_symbols() {
    for (size_t i = 0; i < symbol_count; i++) {
        free(symbols[i].name);
    }
}

void register_symbols(const char *filename) {
    ASSERT(elf_version(EV_CURRENT) != EV_NONE);

    int fd = open(filename, O_RDONLY, 0);
    if (fd < 0) {
        debug("Couldn't open file descriptor %s\n", filename);
        return;
    }

    Elf *elf = elf_begin(fd, ELF_C_READ, NULL);
    if (!elf) {
        debug("Couldn't open ELF file %s\n", filename);
        close(fd);
        return;
    }

    // Get the ELF header
    GElf_Ehdr ehdr;
    if (gelf_getehdr(elf, &ehdr) == NULL) {
        debug("Couldn't get the ELF header %s\n", filename);
        elf_end(elf);
        close(fd);
        return;
    }

    // Locate the section header string table
    Elf_Scn *scn = NULL;
    GElf_Shdr shdr;
    size_t shstrndx;

    if (elf_getshdrstrndx(elf, &shstrndx) != 0) {
        debug("Couldn't locate the section name string table %s\n", filename);
        elf_end(elf);
        close(fd);
        return;
    }

    // Iterate through sections to find the symbol table
    while ((scn = elf_nextscn(elf, scn)) != NULL) {
        if (gelf_getshdr(scn, &shdr) != &shdr) {
            debug("Couldn't find the symbol table %s", filename);
            elf_end(elf);
            close(fd);
            return;
        }

        if (shdr.sh_type == SHT_SYMTAB || shdr.sh_type == SHT_DYNSYM) {
            // Get string table for symbols
            Elf_Data *data = NULL;
            size_t n_symbols = shdr.sh_size / shdr.sh_entsize;

            while ((data = elf_getdata(scn, data)) != NULL) {
                for (size_t i = 0; i < n_symbols; i++) {
                    GElf_Sym sym;
                    if (gelf_getsym(data, i, &sym) != &sym) {
                        continue;
                    }

                    const char *name =
                        elf_strptr(elf, shdr.sh_link, sym.st_name);
                    if (!name) {
                        continue;
                    }

                    add_symbol(name, sym.st_value);
                }
            }
        }
    }

    elf_end(elf);
    close(fd);
}
