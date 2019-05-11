/*
 *  This file is part of the SGX-Step enclave execution control framework.
 *
 *  Copyright (C) 2017 Jo Van Bulck <jo.vanbulck@cs.kuleuven.be>,
 *                     Raoul Strackx <raoul.strackx@cs.kuleuven.be>
 *
 *  SGX-Step is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SGX-Step is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SGX-Step. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <fcntl.h>
#include "file.h"
#include "debug.h"

int file_read_offset(const char *path, uint8_t *buf, int len, off_t offset)
{
    int rv, fd;

    libsgxstep_info("reading buffer from '%s' (size=%d)", path, len);
    ASSERT( (fd = open(path, O_RDONLY)) >= 0 );
    rv = pread(fd, buf, len, offset);
    close(fd);

    return rv;
}

int file_read(const char *path, uint8_t *buf, int len)
{
    return file_read_offset(path, buf, len, 0);
}

int file_write_offset(const char *path, uint8_t *buf, int len, off_t offset)
{
    int rv, fd;

    libsgxstep_info("writing buffer to '%s' (size=%d)", path, len);
    ASSERT( (fd = open(path, O_WRONLY)) >= 0 );
    rv = pwrite(fd, buf, len, offset);
    close(fd);

    return rv;
}

int file_write(const char *path, uint8_t *buf, int len)
{
    return file_write_offset(path, buf, len, 0);
}

int file_read_int(const char *path, int *data)
{
    int rv;
    FILE *f;

    ASSERT( (f = fopen(path, "r")) );
    ASSERT( (rv = fscanf(f, "%i", data)) == 1);
    fclose(f);

    return rv;
}

int file_write_int(const char *path, int data)
{
    int rv;
    FILE *f;

    ASSERT( (f = fopen(path, "w")) );
    rv = fprintf(f, "%i", data);
    fclose(f);

    return rv;
}
