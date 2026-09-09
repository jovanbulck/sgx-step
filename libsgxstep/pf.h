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

#ifndef SGX_STEP_PF_H
#define SGX_STEP_PF_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef void (*fault_handler_t)(size_t page_num);
void register_fault_handler(fault_handler_t cb);
int protect_pages(size_t page, size_t num_pages, int prot);
int revoke_pages(size_t page, size_t num_pages);
int restore_pages(size_t page, size_t num_pages);

#endif // SGX_STEP_PF_H
