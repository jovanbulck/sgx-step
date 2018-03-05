#include <stdio.h>
#include "file.h"
#include "debug.h"

int file_read(const char *path, uint8_t *buf, int len)
{
    int rv;
    FILE *f;

    info("reading buffer from '%s' (size=%d)", path, len);
    ASSERT( (f = fopen(path, "rb")) );
    rv = fread(buf, len, 1, f);
    fclose(f);

    return rv;
}

int file_write(const char *path, uint8_t *buf, int len)
{
    int rv;
    FILE *f;

    info("writing buffer to '%s' (size=%d)", path, len);
    ASSERT( (f = fopen(path, "wb")) );
    rv = fwrite(buf, len, 1, f);
    fclose(f);

    return rv;
}
