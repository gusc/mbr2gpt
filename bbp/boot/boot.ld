OUTPUT_FORMAT(elf64-x86-64)
SECTIONS {
	.text : {
		boot.s.o(.text);
		bios.s.o(.text);
		main16.c64.o(.text);
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