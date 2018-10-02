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

	.globl rtm_begin
	.type rtm_begin, @function
	.section .text
rtm_begin:
	xbegin 1f
	mov $0, %rax
	retq
1:
	mov $-1, %rax
	retq

	.globl rtm_end
	.type rtm_end, @function
	.section .text
rtm_end:
	xend
	retq

	.globl rtm_abort
	.type rtm_abort, @function
	.section .text
rtm_abort:
	xabort $0
	retq



