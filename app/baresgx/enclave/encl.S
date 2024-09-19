	.section ".tcs", "aw"
	.balign	4096

	.fill	1, 8, 0			# STATE (set by CPU)
	.fill	1, 8, 0			# FLAGS
	.quad	encl_ssa_tcs1		# OSSA
	.fill	1, 4, 0			# CSSA (set by CPU)
	.fill	1, 4, 1			# NSSA
	.quad	encl_entry		# OENTRY
	.fill	1, 8, 0			# AEP (set by EENTER and ERESUME)
	.fill	1, 8, 0			# OFSBASE
	.fill	1, 8, 0			# OGSBASE
	.fill	1, 4, 0xFFFFFFFF 	# FSLIMIT
	.fill	1, 4, 0xFFFFFFFF	# GSLIMIT
	.fill	4024, 1, 0		# Reserved

        /* 
         * Minimal test enclave with a single code page with exactly 100
         * instructions.
         */
	.text
        .align 0x1000
encl_entry:
        mov     encl_secret(%rip), %rsi

	.rept 96
		nop
	.endr

	# EEXIT
        mov     %rcx, %rbx
	mov	$4, %rax
	enclu

        .data
encl_ssa_tcs1:
	.space 4096

encl_secret:
        .quad 0xdeadbeefcafebabe

