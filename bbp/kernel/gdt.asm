;
; GDT wrapper
; ===========
;
; Wrapper routine for GDT setup
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
[bits 32]
[global gdt_set]								; Export gdt_set to C

gdt_set:										; prototype: void gdt_set(uint32 gdt_ptr)
	push ebp									; store current ebp on the stack
	mov ebp, esp								; copy esp to ebp
	mov eax, [ebp + 8]							; get the pointer to the GDT, passed as a parameter.
	lgdt [eax]									; load the new GDT

	mov ax, 0x0010								; selector 0x10 - data descriptor
	mov ds, ax									; set Data Segment
	mov es, ax									; set Extra Segment
	mov fs, ax									; set Data2 Segment
	mov gs, ax									; set Data3 Segment
	mov ss, ax									; set Stack Segment
	jmp 0x08:.end								; do the magic jump to finalize GDT setup
.end:
	pop ebp										; restore ebp
	ret											; return to C
