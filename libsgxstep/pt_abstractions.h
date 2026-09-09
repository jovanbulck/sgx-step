#ifndef SGX_STEP_PT_ABSTRACTIONS_H
#define SGX_STEP_PT_ABSTRACTIONS_H

#include <stdint.h>
#include <stddef.h>
#include "enclave.h"
#include "pt.h"

int pte_revoke_pages(size_t page, size_t num_pages);

int pte_restore_pages(size_t page, size_t num_pages);

uint64_t *get_pte_by_page(size_t page);

uint64_t *get_pte_by_address(void *virt_addr);

// Converts a virtual address into the corresponding page number
static inline size_t virt_to_pagenum(void *virt)
{
    return ((size_t)virt - (size_t)get_enclave_base()) >> PT_SHIFT;
}

#endif // SGX_STEP_PT_ABSTRACTIONS_H