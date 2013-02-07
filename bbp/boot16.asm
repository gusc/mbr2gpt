; This is an entry point for our BBP code
;
; Program flow:
; 1. setup registers
; 2. set video mode to teletype (80 x 25)
; 3. enable A20 Gate
; 4. enable Protected Mode
; 5. setup segments
; 6. execute C main32() method that sets up IDT etc.
; 7. setup PML4 and Paging
; 8. enable Long Mode
; 9. execute C main64() method which should start up full long mode environment

%define ORG_LOC 0x7C00							; Initial position in memory (where MBR loads us)
%define	E820_LOC		0x0800					; Location of E820 Memory Map

[section .data]

[global gdt]									; Make GDT accessible from C
[global gdt_ptr]								; Make GDT pointer accessible from C
[global idt]									; Make IDT accessible from C
[global idt_ptr]								; Make IDT pointer accessible from C
[global pml4_ptr]								; Make PML4 pointer accessible from C

; Global Descriptor Table (GDT) used to do a Protected Mode jump
gdt:
; Null Descriptor (selector: 0x00)
.null_desc:
	dw 0x0000
	dw 0x0000
	db 0x00
	db 0x00
	db 0x00
	db 0x00

; Code Descriptor (selector: 0x08)
.code_desc:
	dw 0xffff									; Limit
	dw 0x0000									; Base (low word)
	db 0x00										; Base (high word low byte)
	db 0x9a										; Access byte
	db 0xcf										; Limit (high nibble, 4bits) + flags (nibble)
	db 0x00										; Base (high word high byte)

; Data Descriptor (selector: 0x10)
.data_desc:
	dw 0xffff									; Limit
	dw 0x0000									; Base (low word)
	db 0x00										; Base (high word low byte)
	db 0x92										; Access byte
	db 0xcf										; Limit (high nibble, 4bits) + flags (nibble)
	db 0x00										; Base (high word high byte)
gdt_end:

; GDT pointer
gdt_ptr:
	dw (gdt_end - gdt - 1)						; Limit (size)
	dd (gdt + 0x0000)							; Base (location)
gdt_ptr_end:

; Interrupt Descriptor Table (IDT)
idt:
	times (64 * 256) db 0						; Dummy entries - we'll populate them main32
idt_end:

; IDT pointer
idt_ptr:
	dw (idt_end - idt - 1)						; Limit (size)
	dd (idt + 0x0000)							; Base (location)
idt_ptr_end:

; PML4 pointer
pml4_ptr:
	dd 0										; Dummy entry - we'll populate it in main32
pml4_ptr_end:

[section .text]
[global start16]								; Export start16 to linker
[extern main32]									; Import main32() from C
[extern main64]									; Import main64() from C

; Remember in NASM it's:
; instruction destination, source

[bits 16]										; Real mode

start16:										; Our entry point
	xor eax, eax								; clear ax
	mov ss, ax									; set stack segment to zero (is it, i'm dumb in assembly?)
	mov sp, ORG_LOC								; set stack pointer to the begining of MBR location in memory
	mov es, ax									; zero-out extra segment
	mov ds, ax									; zero-out data segment

set_video:										; Set video mode
	mov ah, 0x00								; command: change video mode
	mov al, 0x03								; 3rd Mode -> 80x25
	int 0x10									; call video interrupt

set_a20:										; Enable A20 Gate to access high memory in protected mode (Fast A20)
	cli											; disable interrupts
	in al, 0x92									; read from io port 0x92
	test al, 0x02								; do some test
	out 0x90, al								; write to io port 0x90

e820_map:										; Read E820 Memory map
	mov di, E820_LOC + 4						; set DI to location where we'll read E820 map
	mov eax, 0x0000E820							; command: E820 map
	xor ebx, ebx								; clear ebx
	mov ecx, 0x18								; set ECX to value of 24 - the max size of entry (only ACPI 3.0 has 24 byte entries)
	mov edx, 0x534D4150							; set EDX to magic number
	int 0x15									; call memory interrupt?
	jc .end_fail								; if carry flag set, then we failed
	cmp eax, 0x534D4150							; should be the magic number, is it?
	jne .end_fail								; if magic number is not set, then we failed
	push 1										; store entry count on the stack
.read_more:
	cmp ebx, 0x0								; test EBX for 0
	je .end_ok									; if EBX is 0, this was the last entry
	pop ax										; get entry count from the stack
	inc ax										; increment entry count on the stack
	push ax										; store entry count on the stack
	mov [di - 2], cx							; save entry size
	add di, cx									; increment DI by entry size
	add di, 2									; increment DI by entry size offset (word size)
	mov eax, 0x0000E820							; command: E820 map
	int 0x15									; call memory interrupt?
	jc .end_ok									; if carry flag set, then we failed
	jmp .read_more								; read some more

.end_fail:
	xor eax, eax								; clear EAX
	mov [E820_LOC], ax							; no entries on E820 map
	jmp load_gdt								; continiue to GDT loader

.end_ok:
	pop ax										; get entry count
	mov [E820_LOC], ax							; save entry count before our memory structure
	
load_gdt:										; Load Global Descriptor Table
	lgdt [gdt_ptr]								; gdt_ptr is an address to memory so we enclose it in []
	
protected_mode:									; Enter Protected Mode
	mov eax, cr0								; read from Control Register CR0
	or eax, 0x0001								; set Protected Mode bit
	mov cr0, eax								; write to Control Register CR
	jmp 0x8:start32								; do the magic jump to finalize Protected Mode setup

[bits 32]										; Protected mode

start32:										; Setup all the data segments
	mov eax, 0x0010								; selector 0x10 - data descriptor
	mov ss, ax									; set Stack Segment
	mov ds, ax									; set Data Segment
	mov es, ax									; set Extra Segment
	mov fs, ax									; set Data2 Segment
	mov gs, ax									; set Data3 Segment
	sti											; enable interrupts
	call main32									; call C function main32() (see: boot32/main32.c)
	cli											; disable interrupts

; If we can not do it in C we'll try to do it in ASM here:

disable_paging:									; Disable paging
	mov eax, cr0								; read from Control Register CR0
	and eax, 0x7FFFFFFF							; clear Paging enabled (PE) bit
	mov cr0, eax								; write to Control Register CR0
	
enable_pae:										; Enable PAE
	mov eax, cr4								; read from Control Registar CR4
	or eax, 0x0050								; set PAE and PGE bits
	mov cr4, eax								; write to Control Register CR4

set_pml4_ptr:
	mov eax, pml4_ptr							; copy PML4 pointer address to eax
	mov cr3, eax								; set PML4 pointer address in CR3

ia32e_mode:										; Enter Long Mode
	mov ecx, 0xC0000080							; set to work with EFER MSR
	rdmsr 										; read EFER MSR
	or edx, 0x80								; set Long mode enabled (LME) bit
	wrmsr										; write EFER MSR

enable_paging:
	mov eax, cr0								; read from Control Register CR0
	or eax, 80000000							; set Paging bit
	mov cr0, eax								; write to Control Register CR0

reload_gdt:										; Re-Load Global Descriptor Table
	lgdt [gdt_ptr]								; gdt_ptr is an address to memory so we enclose it in []
	jmp 0x8:start64								; do the magic jump to finalize Long Mode setup

[bits 64]										; Long mode

start64:										; Transfer control to C
	mov rax, 0x0010								; selector 0x10 - data descriptor
	mov ss, ax									; set Stack Segment
	mov ds, ax									; set Data Segment
	mov es, ax									; set Extra Segment
	mov fs, ax									; set Data2 Segment
	mov gs, ax									; set Data3 Segment
	sti											; enable interrupts
	call main64									; call C function main64() (see: boot64/main64.c)
	cli											; disable interrupts
	jmp $										; hang
