#ifndef LIBSGXSTEP_FILE_H_INC
#define LIBSGXSTEP_FILE_H_INC

#include <stdint.h>

int file_read(const char *path, uint8_t *buf, int len);
int file_write(const char *path, uint8_t *buf, int len);

#endif
