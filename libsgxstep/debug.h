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

#ifndef SGX_STEP_DEBUG_H
#define SGX_STEP_DEBUG_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "config.h"

extern int sgx_step_rv;

#define SGX_ASSERT(f)  { if ( 0 != (sgx_step_rv = (f)) )                \
 {                                                                      \
       printf( "Error calling enclave at %s:%d (rv=0x%x)\n", __FILE__,  \
                                              __LINE__, sgx_step_rv);   \
        abort();                                                        \
 } }

#define ASSERT(cond)                                                    \
    do {                                                                \
        if (!(cond))                                                    \
        {                                                               \
            perror("[" __FILE__ "] assertion '" #cond "' failed");      \
            tear_down_sgx_step();                                       \
            abort();                                                    \
        }                                                               \
    } while(0)

#define WARN_ON(cond, msg)                                              \
    do {                                                                \
        if ((cond))                                                     \
        {                                                               \
            info("WARNING: %s", msg);                                   \
        }                                                               \
    } while(0)

#define info(msg, ...)                                                  \
    do {                                                                \
        printf("[" __FILE__ "] " msg "\n", ##__VA_ARGS__);              \
        fflush(stdout);                                                 \
    } while(0)

#if LIBSGXSTEP_DEBUG
    #define debug(msg, ...)     info("DEBUG: " msg, ##__VA_ARGS__)
#else
    #define debug(msg, ...)
#endif


#if LIBSGXSTEP_SILENT
    #define libsgxstep_info(msg, ...)
#else
    #define libsgxstep_info(msg, ...) info(msg, ##__VA_ARGS__)
#endif

#define info_event(msg, ...)                                                                        \
do {                                                                                                \
    printf("\n--------------------------------------------------------------------------------\n"); \
    info(msg,##__VA_ARGS__);                                                                        \
    printf("--------------------------------------------------------------------------------\n\n"); \
} while(0)

void dump_hex(uint8_t *buf, int len);
void tear_down_sgx_step( void );

#endif
