/*OUTPUT_FORMAT(elf64-x86-64)*/
OUTPUT_FORMAT(binary)
ENTRY(start16)
SECTIONS {
	.text 0x7C00 : AT( 0x7C00 ) {
		boot16_64.o(.text); 
		*(.text);
	}
	.rodata : {
		*(.rodata);
	}
	.data : {
		*(.data);
	}
	/DISCARD/ : {
		*(.eh_frame);
		*(.bss);
	}
	. = ALIGN(512);
}