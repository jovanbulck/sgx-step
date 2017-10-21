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

#ifndef SGXSTEP_INTERNAL_H_INC
#define SGXSTEP_INTERNAL_H_INC

#include <linux/module.h>
#include <linux/kernel.h>

#define DEV           "sgx-step"
#define log(msg, ...) printk(KERN_INFO  "[" DEV "] " msg "\n", ##__VA_ARGS__)
#define err(msg, ...) printk(KERN_ALERT "[" DEV "] error: " msg "\n", ##__VA_ARGS__)

#define RET_ASSERT(cond)                            \
    do {                                            \
        if (!(cond))                                \
        {                                           \
            err("assertion '" #cond "' failed.");   \
            return -EINVAL;                         \
        }                                           \
    } while(0)

/* NOTE: incorrect GPRSGX size in Intel manual vol. 3D June 2016 p.38-7 */
#define SGX_TCS_OSSA_OFFSET         16
#define SGX_GPRSGX_SIZE             184
#define SGX_GPRSGX_RIP_OFFSET       136

/* HACK: to avoid having to retrieve the SSA framesize from the untrusted
   runtime (driver), we assume a standard/hard-coded SSA framesize of 1 page */
#define SGX_SSAFRAMESIZE            4096

#endif
