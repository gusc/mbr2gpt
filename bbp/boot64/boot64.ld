OUTPUT_FORMAT(elf64-x86-64)
SECTIONS{
	.text : { 
		*(.text);
	}
	.rodata : { 
		*(.rodata);
	}
	.data : { 
		*(.data);
	}
	. = ALIGN(4096);
}