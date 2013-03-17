;
; BBP programm entry
; ==================
;
; This file contains entry point and glue code between real mode and long mode
;
; Program flow:
; 1. setup registers
; 2. execute C main16() function that sets up everything to enter protected mode
; 3. enter Protected Mode
; 4. execute C main32() function that sets up everything to enter long mode
; 3. enter Long Mode
; 4. execute C kmain() function that starts up the kernel
;
; License (BSD-3)
; ===============
;
; Copyright (c) 2013, Gusts 'gusC' Kaksis <gusts.kaksis@gmail.com>
; All rights reserved.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;    * Redistributions of source code must retain the above copyright
;      notice, this list of conditions and the following disclaimer.
;    * Redistributions in binary form must reproduce the above copyright
;      notice, this list of conditions and the following disclaimer in the
;      documentation and/or other materials provided with the distribution.
;    * Neither the name of the <organization> nor the
;      names of its contributors may be used to endorse or promote products
;      derived from this software without specific prior written permission.
;
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
; ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
; DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
; (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
; ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
; SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;

; Definitions:
%define ORG_LOC		0x7C00						; Initial position in memory (where MBR loads us)

[section .text]
[global start16]								; Export start16 to linker
[extern main16]									; Import main16() from C
[extern main32]									; Import main32() from C
[extern kmain]									; Import kmain() from C

; Remember in NASM it's:
; instruction destination, source

[bits 16]										; Real mode

start16:										; Boot entry point
	; Setup registers
	xor eax, eax								; clear EAX
	xor ebx, ebx								; clear EBX
	xor ecx, ecx								; clear ECX
	xor edx, edx								; clear EDX
	xor esi, esi								; clear ESI
	xor edi, edi								; clear EDI
	xor ebp, ebp								; clear EBP
	mov ds, ax									; zero out data segment register (DS)
	mov es, ax									; zero out extra segment register (ES)
	mov ss, ax									; zero out stack segment register (SS)
	mov fs, ax									; zero out general purpose data segment register (FS)
	mov gs, ax									; zero out general purpose data segment register (FS)
	mov sp, ORG_LOC								; set stack pointer to the begining of MBR location in memory
	
	; Call Real Mode initialization
	cli											; disable all maskable interrupts
	call main16									; call C function main16() (see: boot/main16.c)
	
	; Enter Protected mode
	lgdt [gdt_ptr]								; load 32bit GDT pointer
	mov eax, cr0								; read from CR0
	or eax, 0x0001								; set Protected Mode bit
	mov cr0, eax								; write to CR0
	jmp 0x8:start32								; do the magic jump to finalize Protected Mode setup

[bits 32]										; Protected mode

start32:										; Protected mode entry point
	; Setup segment registers
	mov eax, 0x10								; selector 0x10 - data descriptor
	mov ss, ax									; set stack segment
	mov ds, ax									; set data segment
	mov es, ax									; set extra segment
	mov fs, ax									; set general purpose data Segment
	mov gs, ax									; set general purpose data Segment

	; Call Protected Mode Initialization
	call main32									; call C function main32() (see: boot/main32.c)
	
	; Disable all IRQs
	mov al, 0xFF								; set out 0xFF to 0xA1 and 0x21 to disable all IRQs
    out 0xA1, al
    out 0x21, al

	; Pause
    nop
    nop

	; Load IDT (empty pointer for now  so that any NMI causes a triple fault)										
    lidt [idt_ptr]								; load 32bit IDT pointer

    ; Setup long mode.
	mov eax, cr4								; read from CR4
    or eax, 0x000000A0							; set the PAE and PGE bit
    mov cr4, eax								; write to CR4
    mov edx, [pml4_ptr]							; point edx to PML4 pointer location
    mov cr3, edx								; save this into CR3
    mov ecx, 0xC0000080							; read from the EFER MSR
    rdmsr										; read MSR
    or eax, 0x00000100							; set the LME bit
    wrmsr										; write MSR

	; Activate Long Mode
    mov ebx, cr0								; read from CR0
	or ebx,0x80000000							; set enabling paging bit
    mov cr0, ebx								; write to CR0
    lgdt [gdt64_ptr]							; load 64bit GDT pointer
	jmp 0x8:start64								; do the magic jump to Long Mode
	
[bits 64]										; Long mode

start64:										; Long Mode entry point
	; Register cleanup
	xchg bx, bx
	xor rax, rax								; aka r0
	xor rbx, rbx								; aka r1
	xor rcx, rcx								; aka r2
	xor rdx, rdx								; aka r3
	xor rsi, rsi								; aka r4
	xor rdi, rdi								; aka r5
	xor rbp, rbp								; aka r6
	mov rsp, ORG_LOC							; aka r7
	xor r8, r8
	xor r9, r9
	xor r10, r10
	xor r11, r11
	xor r12, r12
	xor r13, r13
	xor r14, r14
	xor r15, r15
	mov ds, ax									; clear the legacy segment registers
	mov es, ax
	mov ss, ax
	mov fs, ax
	mov gs, ax

	;sti											; enable interrupts
	call kmain									; call C function kmain() (see: kernel/kmain.c)
	;cli											; disable interrupts
	jmp $										; hang

[section .data]

[global idt]									; Make IDT accessible from C
[global idt_ptr]								; Make IDT pointer accessible from C
[global pml4_ptr]								; Make PML4 pointer accessible from C

; Interrupt Descriptor Table (IDT)
idt:
	times (64 * 256) db 0						; Dummy entries - we'll populate them kmain
idt_end:

; IDT pointer (zero-pointer)
idt_ptr:
	dw 0										; Limit (size)
	dd 0										; Base (location)
idt_ptr_end:

; PML4 pointer
pml4_ptr:
	dd 0										; Dummy entry - we'll populate it in main32 and later in kmain
pml4_ptr_end:

[section .rodata]

; Global Descriptor Table (GDT) used to do the Protected Mode jump (this is read-only as we don't need to update it)
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
	dw 0xffff									; 0:15 - Limit
	dw 0x0000									; 16:31 - Base (low word)
	db 0x00										; 32:39 - Base (high word low byte)
	db 10011010b								; 40:47 - Access byte
	db 11001111b								; 48:55 - Limit (high nibble) + Flags (4 bits) 
	db 0x00										; 56:64 - Base (high word high byte)

; Data Descriptor (selector: 0x10)
.data_desc:
	dw 0xffff									; 0:15 - Limit
	dw 0x0000									; 16:31 - Base (low word)
	db 0x00										; 32:39 - Base (high word low byte)
	db 10010010b								; 40:47 - Access byte
	db 11001111b								; 48:55 - Limit (high nibble) + Flags (4 bits) 
	db 0x00										; 56:64 - Base (high word high byte)
gdt_end:

; GDT pointer (this get's passed to LGDT)
gdt_ptr:
	dw (gdt_end - gdt - 1)						; Limit (size)
	dd (gdt + 0x000000000000)					; Base (location)
gdt_ptr_end:

; 64bit GDT
gdt64:
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
	dw 0xffff									; 0:15 - Limit
	dw 0x0000									; 16:31 - Base (low word)
	db 0x00										; 32:39 - Base (high word low byte)
	db 10011000b								; 40:47 - Access byte
	db 00100000b								; 48:55 - Limit (high nibble) + Flags (4 bits) 
	db 0x00										; 56:64 - Base (high word high byte)

; Data Descriptor (selector: 0x10)
.data_desc:
	dw 0xffff									; 0:15 - Limit
	dw 0x0000									; 16:31 - Base (low word)
	db 0x00										; 32:39 - Base (high word low byte)
	db 10010000b								; 40:47 - Access byte
	db 00000000b								; 48:55 - Limit (high nibble) + Flags (4 bits) 
	db 0x00										; 56:64 - Base (high word high byte)
gdt64_end:

gdt64_ptr:
	dw (gdt64_end - gdt64 - 1)					; Limit (size)
	dq (gdt64 + 0x0000000000000000)				; Base (location)
gdt64_ptr_end: