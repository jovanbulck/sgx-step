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

#ifndef LIBSGXSTEP_FILE_H_INC
#define LIBSGXSTEP_FILE_H_INC

#include <stdint.h>
#include <unistd.h>

int file_read(const char *path, uint8_t *buf, int len);
int file_read_offset(const char *path, uint8_t *buf, int len, off_t offset);

int file_write(const char *path, uint8_t *buf, int len);
int file_write_offset(const char *path, uint8_t *buf, int len, off_t offset);

int file_read_int(const char *path, int *data);
int file_write_int(const char *path, int data);

#endif
