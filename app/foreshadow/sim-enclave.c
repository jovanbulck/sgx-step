#include <stdint.h>
// read entire cache line
#define CACHE_LINE_SIZE     64
#ifndef SECRET_BYTES
    #define SECRET_BYTES    CACHE_LINE_SIZE
#endif

// first few cache lines seem not to work stable (?)
#define SECRET_CACHE_LINE   27
#define SECRET_OFFSET       (CACHE_LINE_SIZE*SECRET_CACHE_LINE)

uint8_t __attribute__ ((aligned(0x1000))) array[1000];
#define secret array[SECRET_OFFSET]

void *sim_generate_secret( void )
{
    for (int i =0; i < SECRET_BYTES; i++)
        array[SECRET_OFFSET+i] = rand();
    return &secret;
}

void sim_destroy_secret( uint8_t cl[64])
{
    uint8_t rv = secret;

    for (int i=0; i < SECRET_BYTES; i++)
    {
        cl[i] = array[SECRET_OFFSET+i];
        array[SECRET_OFFSET+i] = 0xff;
    }
}

void sim_reload( void *adrs )
{
    asm volatile (
        "movl (%0), %%eax\n\t"
        : : "c" (adrs)
        : "%rax");
}

