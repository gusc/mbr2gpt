OUTPUT_FORMAT(elf64-x86-64)
SECTIONS {
	.text : { 
		*(.text);
	}
	.rodata : { 
		*(.rodata);
	}
	.data : { 
		*(.data);
	}
	.bss : {
		*(.bss);
	}
	. = ALIGN(4096);
}