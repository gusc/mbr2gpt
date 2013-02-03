/*OUTPUT_FORMAT(elf32-i386)*/
OUTPUT_FORMAT(binary)
ENTRY(start16)
SECTIONS {
	.text 0x7C00 : AT( 0x7C00 ) { 
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