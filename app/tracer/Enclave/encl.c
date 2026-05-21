#include <stdint.h>
#include <string.h>

char __attribute__((aligned(0x1000))) page_a[4096];
char __attribute__((aligned(0x1000))) page_b[4096];

inline void __attribute__((always_inline)) mwrite(void *p)
{
    asm volatile("movb $0, (%0)\n" : : "r"(p) :);
}

void dummy_enclave_fn(int s)
{
    for ( int i = s; i > 0; i >>= 1)
    {
        if (i & 0x01) 
        {
            mwrite(page_a);        
        }
        else
        {
            mwrite(page_b);
        }
    }
}
