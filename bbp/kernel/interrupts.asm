;
; Helper functions for operations interrupts
; ==================
;
; This contains imports from ASM stub subroutines that catch all the neccessary
; exception interrupts and calls a C function interrupt_handler().
; It also contains LIDT wrapper function.
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
[bits 64]
[extern isr_handler]							; Import int_handler from C
[extern irq_handler]							; Import irq_handler from C
[global idt_set]								; Export void idt_set(idt_ptr_t *idt) to C

; Macro to create an intterupt service routine for interrupts that do not pass error codes 
%macro INT_NO_ERR 1
[global isr%1]
isr%1:
	cli											; disable interrupts
	push qword 0								; set error code to 0
	push qword %1								; set interrupt number
	call isr_handler							; call void isr_handler(int_stack_t args)
	sti											; enable interrupts
	add rsp, 16									; cleanup stack
	iretq										; return from interrupt handler
%endmacro

; Macro to create an interrupt service routine for interrupts that DO pass an error code
%macro INT_HAS_ERR 1
[global isr%1]
isr%1:
	cli											; disable interrupts
	push qword %1								; set interrupt number
	call isr_handler							; call void isr_handler(int_stack_t args)
	sti											; enable interrupts
	add rsp, 16									; cleanup stack
	iretq										; return from interrupt handler
%endmacro

; Macro to create an IRQ interrupt service routine
; First parameter is the IRQ number
; Second parameter is the interrupt number it is remapped to
%macro IRQ 2
[global irq%1]
irq%1:
	cli											; disable interrupts
	push qword %1								; set IRQ number in the place of error code (see registers_t in interrupts.h)
	push qword %2								; set interrupt number
	call irq_handler							; calls void irq_handler(int_stack_t args)
	sti											; enable interrupts
	add rsp, 16									; cleanup stack
	iretq										; return from interrupt handler
%endmacro

idt_set:										; prototype: void idt_set(uint32 idt_ptr)
	cli											; disable interrupts
	lidt [rdi]									; load the IDT (x86_64 calling convention - 1st argument goes into RDI)
	sti											; enable interrupts
	ret											; return to C

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
