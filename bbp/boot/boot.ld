OUTPUT_FORMAT(elf64-x86-64)
SECTIONS {
	.text : {
		boot.s.o(.text);
		bios.s.o(.text);
		main16.c64.o(.text);
		*(.text);
	}
	.data : { 
		*(.data);
	}
	.rodata : { 
		*(.rodata);
	}
	.bss : {
		*(.bss);
	}
	. = ALIGN(1024);
}