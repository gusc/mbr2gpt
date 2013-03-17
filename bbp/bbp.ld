/*OUTPUT_FORMAT(elf64-x86-64)*/
OUTPUT_FORMAT(binary)
ENTRY(start16)
SECTIONS {
	.text 0x7C00 : AT( 0x7C00 ) {
		boot.o(.text); 
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
	/DISCARD/ : {
		*(.eh_frame);
	}
	. = ALIGN(1024);
}