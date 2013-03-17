;
; Interrupt wrappers
; ==================
;
; This file contains Interrupt wrapper subroutine that calls a C function interrupt_handler()
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

[section .text]

[extern isr_handler]							; Import int_handler from C
[extern irq_handler]							; Import irq_handler from C

; Macro to create an intterupt service routine for interrupts that do not pass error codes 
%macro INT_NO_ERR 1
[global isr%1]
isr%1:
	cli											; disable interrupts
	push byte 0									; set error code to 0
	push byte %1								; set interrupt number
	jmp isr_wrapper								; jump to our local wrapper stub
%endmacro

; Macro to create an interrupt service routine for interrupts that DO pass an error code
%macro INT_HAS_ERR 1
[global isr%1]
isr%1:
	cli											; disable interrupts
	push byte %1								; set interrupt number
	jmp isr_wrapper								; jump to our local wrapper stub
%endmacro

; Macro to create an IRQ interrupt service routine
; First parameter is the IRQ number
; Second parameter is the interrupt number it is remapped to
%macro IRQ 2
[global irq%1]
irq%1:
	cli											; disable interrupts
	push byte %1								; set IRQ number in the place of error code (see registers_t in interrupts.h)
	push byte %2								; set interrupt number
	jmp irq_wrapper								; jump to our local wrapper stub
%endmacro

; Setup all the neccessary service routines with macros
INT_NO_ERR 0
INT_NO_ERR 1
INT_NO_ERR 2
INT_NO_ERR 3
INT_NO_ERR 4
INT_NO_ERR 5
INT_NO_ERR 6
INT_NO_ERR 7
INT_NO_ERR 8 ; By all specs int 8 is double fault WITH error message, but not in Bochs, WHY?
INT_NO_ERR 9
INT_HAS_ERR 10
INT_HAS_ERR 11
INT_HAS_ERR 12
INT_HAS_ERR 13
INT_HAS_ERR 14
INT_NO_ERR 15
INT_NO_ERR 16
INT_NO_ERR 17
INT_NO_ERR 18
INT_NO_ERR 19
INT_NO_ERR 20
INT_NO_ERR 21
INT_NO_ERR 22
INT_NO_ERR 23
INT_NO_ERR 24
INT_NO_ERR 25
INT_NO_ERR 26
INT_NO_ERR 27
INT_NO_ERR 28
INT_NO_ERR 29
INT_NO_ERR 30
INT_NO_ERR 31
IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

isr_wrapper:									; Local interrupt handler wrapper
	pusha										; push all the register on the stack edi,esi,ebp,esp,ebx,edx,ecx,eax
	mov ax, ds									; store ds in lower 16-bits of eax
	push eax									; save the data segment descriptor
	mov ax, 0x10								; load the kernel data segment descriptor
	mov ds, ax									; set Data Segment
	mov es, ax									; set Extra Segment
	mov fs, ax									; set Data2 Segment
	mov gs, ax									; set Data3 Segment

	call isr_handler							; call our isr_handler() C function 

	pop ebx										; reload the original data segment descriptor
	mov ds, bx									; set Data Segment
	mov es, bx									; set Extra Segment
	mov fs, bx									; set Data2 Segment
	mov gs, bx									; set Data3 Segment
	popa										; pop  all the registers from the stack
	add esp, 8									; cleans up the pushed error code and pushed ISR number
	sti											; re-enable interrupts
	iret										; Interrupt return
												; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP

irq_wrapper:									; Local IRQ handler wrapper
	pusha										; push all the register on the stack edi,esi,ebp,esp,ebx,edx,ecx,eax
	mov ax, ds									; store ds in lower 16-bits of eax
	push eax									; save the data segment descriptor
	mov ax, 0x10								; load the kernel data segment descriptor
	mov ds, ax									; set Data Segment
	mov es, ax									; set Extra Segment
	mov fs, ax									; set Data2 Segment
	mov gs, ax									; set Data3 Segment

	call irq_handler							; call our irq_handler() C function 

	pop ebx										; reload the original data segment descriptor
	mov ds, bx									; set Data Segment
	mov es, bx									; set Extra Segment
	mov fs, bx									; set Data2 Segment
	mov gs, bx									; set Data3 Segment
	popa										; pop  all the registers from the stack
	add esp, 8									; cleans up the pushed error code and pushed ISR number
	sti											; re-enable interrupts
	iret										; Interrupt return
												; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP
