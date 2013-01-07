OUTPUT_FORMAT(binary)
ENTRY(start)
SECTIONS{
	.text 0x7C00 : { 
		boot.o(.text)
		kmain.o(.text)
		main.o(.text)
	}
	.rodata : { *(.rodata) }
	.data : { *(.data) }
}