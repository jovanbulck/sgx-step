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
 *
 *  NOTE: code below is adapted from the msr-tools free software (GPL)
 *  project <https://github.com/01org/msr-tools/>.
 *
 */

#include "msr.h"
#include "file.h"
#include <stdio.h>

uint64_t rdmsr_on_cpu(uint32_t reg, int cpu)
{
    uint64_t data;
    int fd;
    char *pat;
    int width;
    char msr_file_name[64];
    unsigned int bits;
    
    sprintf(msr_file_name, "/dev/cpu/%d/msr", cpu);
    file_read_offset(msr_file_name, (uint8_t*) &data, sizeof(data), reg);
    
    return data;
}
