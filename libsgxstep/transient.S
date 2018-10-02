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

	.text
	.global transient_access
    # %rdi: oracle
    # %rsi: secret_ptr
    # %rdx: slot_size
transient_access:

    mov $0, %rax
    tzcnt  %rdx, %rcx             # slot_size <- log2( slot_size )

    #prefetcht0 (%rsi)

retry:
    movb (%rsi), %al
    shl %cl, %rax
    jz retry
    movq (%rdi, %rax), %rdi
    retq
