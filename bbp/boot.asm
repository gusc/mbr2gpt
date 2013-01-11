; This is an entry point for our BBP code
;
; Program flow:
; 1. setup registers
; 2. set video mode to teletype (80 x 25)
; 3. enable A20 Gate
; 4. enable Protected Mode
; 5. setup segments
; 6. execute C kmain() method

%define ORG_LOC			0x7C00					; Initial position in memory (where MBR loads us)

[section .rodata]

[global gdt]									; Make GDT accessible from C
[global gdt_ptr]								; Make GDT pointer accessible from C

; Global Descriptor Table
gdt:
	; Null Descriptor (selector: 0x00)
	dw 0x0000
	dw 0x0000
	db 0x00
	db 0x00
	db 0x00
	db 0x00

	; Code Descriptor (selector: 0x08)
	dw 0xffff		; Limit
	dw 0x0000		; Base (low word)
	db 0x00			; Base (high word low byte)
	db 0x9a			; Access byte
	db 0xcf			; Limit (high nibble, 4bits) + flags (nibble)
	db 0x00			; Base (high word high byte)

	; Data Descriptor (selector: 0x10)
	dw 0xffff		; Limit
	dw 0x0000		; Base (low word)
	db 0x00			; Base (high word low byte)
	db 0x92			; Access byte
	db 0xcf			; Limit (high nibble, 4bits) + flags (nibble)
	db 0x00			; Base (high word high byte)
gdt_end:

; GDT pointer
gdt_ptr:
	dw (gdt_end - gdt - 1)
	dd (gdt + 0x0000)

[section .text]

[global start]									; Make start global
[extern kmain]									; C kmain() function

; Remember in NASM it's:
; instruction destination, source

[bits 16]

start:											; Our entry point
	xor eax, eax								; clear eax
	mov ss, ax									; set stack segment to zero (is it, i'm dumb in assembly?)
	mov sp, ORG_LOC								; set stack pointer to the begining of MBR location in memory
	mov es, ax									; zero-out extra segment
	mov ds, ax									; zero-out data segment

set_video:										; Set video mode
	mov al, 0x03								; 3rd Mode -> 80x25
	int 0x10									; call video Interrupt

set_a20:										; Enable A20 Gate to access high memory in protected mode (Fast A20)
	cli											; disable interrupts
	in al, 0x92									; read from io port 0x92
	test al, 0x02								; do some test
	out 0x90, al								; write to io port 0x90

load_gdt:										; Load Global Descriptor Table
	lgdt [gdt_ptr]								; gdt_ptr is an address to memory so we enclose it in []
	
protected_mode:									; Enter Protected Mode
	mov eax, cr0								; read from Control Register CR0
	or eax, 0x0001								; set Protected Mode bit
	mov cr0, eax								; write to Control Register CR
	; In Real Mode		:	ljmp Segment:Offset
	; In Protected Mode	:	ljmp Selector:Offset
	; Selector:
	;  0x0	= NullDescriptor
	;  0x8	= CodeDescriptor
	;  0x10	= DataDescriptor
	jmp 0x8:setup								; do the magic jump to finalize Protected Mode setup

[bits 32]

setup:											; Setup all the data segments
	mov ax, 0x0010								; selector 0x10 - data descriptor
	mov ds, ax									; set Data Segment
	mov es, ax									; set Extra Segment
	mov fs, ax									; set Data2 Segment
	mov gs, ax									; set Data3 Segment
	mov ss, ax									; set Stack Segment

exec_main:										; Transfer controll to C
	sti											; enable interrupts
	call kmain									; call C function kmain() (see: kmain.c)
	cli											; disable interrupts
	
