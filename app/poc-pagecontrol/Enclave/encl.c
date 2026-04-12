#include "encl_t.h"
#include <sgx_trts.h>
#define PAGE_SIZE 4096
#define AMOUNT_OF_PAGES 2048 // How long is the internal buffer?

// Internal buffer within enclave
static uint8_t internal_buffer[4096 * AMOUNT_OF_PAGES] __attribute__((aligned(PAGE_SIZE * 2)));

void ecall_access_target_page()
{
    volatile uint8_t tmp;
    tmp = internal_buffer[0]; // Access Page
    return;
}

void *ecall_leak_internal_buffer_adrs(void)
{
    return (void *)internal_buffer;
}