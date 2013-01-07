OUTPUT_FORMAT(binary)
ENTRY(start)
SECTIONS{
	.text 0x7C00 : { 
		boot.o(.text)
		*(.text)
	}
	.rodata : { *(.rodata) }
	.data : { *(.data) }
}