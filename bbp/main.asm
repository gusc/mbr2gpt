.section .rodata

msg:
	.asciz "kernel..."
msgend:

msglen:
	.word (msgend - msg)

gdt:
	# Null Descriptor
	.word 0x0000
	.word 0x0000
	.byte 0x00
	.byte 0x00
	.byte 0x00
	.byte 0x00

	# Code Descriptor
	.word 0xffff		# Limit
	.word 0x0000		# Base (low word)
	.byte 0x00			# Base (high word low byte)
	.byte 0x9a			# Access byte
	.byte 0xcf			# Limit (high nibble, 4bits) + flags (nibble)
	.byte 0x00			# Base (high word high byte)

	# Data Descriptor
	.word 0xffff		# Limit
	.word 0x0000		# Base (low word)
	.byte 0x00			# Base (high word low byte)
	.byte 0x92			# Access byte
	.byte 0xcf			# Limit (high nibble, 4bits) + flags (nibble)
	.byte 0x00			# Base (high word high byte)
gdt_end:

gdtptr:
	.word (gdt_end - gdt - 1)
	.long (gdt + 0x0000)

.section .text

.globl Start

Start:
	.code16
	xor %eax, %eax
	movw %ax, %ss
	movw $0x7C00, %sp
	movw %ax, %es
	movw %ax, %ds
	jmp SetVideoMode

SetVideoMode:
	clrb %ah				# Set Video Mode
	movb $0x03, %al			# 3rd Mode -> 80x25
	int $0x10				# Video Interrupt

ClearScreen:
	movb $0x06, %ah			# Clear Screen
	clrb %al				# Lines to scroll (0 = clear -> CX & DX)
	movb $0x0f, %bh			# Color Black White
	clrw %cx				# Upper Left Corner
	movb $25, %dh			# 25 Lines
	movb $80, %dl			# 80 Columns
	int $0x10				# Video Interrupt

PrintMsg:
	movw $msg, %bp			# Set base pointer to msg location
	movb $0x13, %ah			# Print String
	movb $0x01, %al			# Char only - Cursor moved
	clrb %bh				# Page Number
	movb $0x03, %bl			# Color Black Blue
	movw msglen, %cx		# Message Length
	movb $3, %dh			# Row 3 
	movb $1, %dl			# Column 1
	int $0x10				# Video Interrupt

	cli

SetA20:
	inb $0x92, %al			# Enable A20 Gate
	testb $0x2, %al			# to access to more than
	outb %al, $0x92			# 1 Mega Byte of memory

LoadGDT:
	lgdt gdtptr				# Load Global Descriptor Table
	
EnterProtectedMode:
	movl %cr0, %eax			# Read from Control Register CR0
	orl $1, %eax			# Set Protected Mode Bit
	movl %eax, %cr0			# Write to Control Register CR

	# In Real Mode		:	ljmp Segment, Offset
	# In Protected Mode	:	ljmp Selector, Offset
	# Selector:
	#  0x0	= NullDescriptor
	#  0x8	= CodeDescriptor
	#  0x10	= DataDescriptor
	ljmp $0x8, $SetupSelectors

	.code32

SetupSelectors:
	movw $0x0010, %ax		# Selector 0x0010
	movw %ax, %ds			# Set Data Segment
	movw %ax, %es			# Set Extra Segment
	movw %ax, %fs			# Set Data2 Segment
	movw %ax, %gs			# Set Data3 Segment
	movw %ax, %ss			# Set Stack Segment

ExecKMain:
	sti
	call kmain
	cli

Hang:
	jmp Hang
