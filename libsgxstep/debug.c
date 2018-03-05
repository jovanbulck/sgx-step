#include "debug.h"

sgx_status_t sgx_step_rv = SGX_SUCCESS;

void dump_hex(uint8_t *buf, int len)
{
    for (int i=0; i < len; i++)
        printf("%02x", *(buf + i));
    printf("\n");
}
