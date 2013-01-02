OUTPUT_FORMAT(binary)
ENTRY(Start)
SECTIONS{
	.text 0x7c00 : { *(.text) }
	.rodata : { *(.rodata) }
	.data : { *(.data) }
}