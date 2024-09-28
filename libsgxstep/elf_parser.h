#ifndef SGX_STEP_ELF_PARSER_H
#define SGX_STEP_ELF_PARSER_H

#include <stddef.h>
#include <stdint.h>
#define MAX_SYMBOLS 1000

struct symbol {
    char *name;
    uintptr_t address;
};

uintptr_t get_symbol_offset(const char *name);
void register_symbols(const char *filename);

#endif
