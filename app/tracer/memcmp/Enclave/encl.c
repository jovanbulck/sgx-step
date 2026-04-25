#include <stdint.h>
#include <string.h>

char __attribute__((aligned(0x1000))) trigger_page[4096];

char *secret = "SECRET";

inline void __attribute__((always_inline)) mwrite(void *p)
{
    asm volatile("movb $0, (%0)\n" : : "r"(p) :);
}

int my_memcmp(char *a, int a_len, char *b, int b_len)
{
    int i;

    /* first check overall len */
    if (a_len != b_len)
        return 0;

    /* now check individual chars */
    for (i=0; i < a_len; i++)
    {
        if (a[i] != b[i])
            return 0;
    }
    return 1;
}

int memcmp_pwd(char *pwd)
{
    int pwd_len = strlen(pwd);
    int secret_len = strlen(secret);
    mwrite(&trigger_page);
    int rv = my_memcmp(pwd, pwd_len, secret, secret_len);
    mwrite(&trigger_page);
    return rv;
}
