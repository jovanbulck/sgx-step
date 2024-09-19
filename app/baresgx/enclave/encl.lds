OUTPUT_FORMAT(elf64-x86-64)

PHDRS
{
	tcs PT_LOAD;
	text PT_LOAD;
	data PT_LOAD;
}

SECTIONS
{
	. = 0;
        __encl_base = .;
	.tcs : {
		*(.tcs*)
	} : tcs

	. = ALIGN(4096);
	.text : {
		*(.text*)
		*(.rodata*)
	} : text

	. = ALIGN(4096);
	.data : {
		*(.data.encl_buffer)
		*(.data*)
	} : data

	/DISCARD/ : {
		*(.comment*)
		*(.note*)
		*(.debug*)
		*(.eh_frame*)
		*(.dyn*)
		*(.gnu.hash)
	}
}

ASSERT(!DEFINED(_GLOBAL_OFFSET_TABLE_), "Libcalls through GOT are not supported in enclaves")
