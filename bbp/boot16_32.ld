OUTPUT_FORMAT(elf32-i386)
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
	. = ALIGN(512);
}