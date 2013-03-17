;
; BIOS utility subroutines
; ========================
;
; This file contains wrappers for BIOS funciton calls to be used in C.
; All the functions are written with cdecl calling convention
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
[global set_video_mode]							; Export void set_video_mode(uint16 mode) to C
[global set_svga_mode]							; Export void set_svga_mode(uint16 mode) to C
[global enable_a20]								; Export uint32 enable_a20() to C
[global read_e820]								; Export void read_e820(e820map_t *mem_map) to C

[bits 16]										; Real mode

set_video_mode:									; Set video mode
	push bp										; save base pointer
	mov bp, sp									; set stack pointer as our base pointer
	add bp, 4									; increment base pointer (as the first 2 values 
												; on the stack are the old base pointer and
												; old instruction pointer)	
	add bp, 2									; if using .code16gcc add 2 more bytes, as GCC pushes 32bit values on the stack
	mov ax, word [bp]							; get video mode parameter
	mov ah, 0x00								; command: change video mode
	int 0x10									; call video interrupt
	pop bp										; restore the old base pointer
	ret											; return to callee

set_svga_mode:									; Set SuperVGA video mode
	push bp										; save base pointer
	mov bp, sp									; set stack pointer as our base pointer
	add bp, 4									; increment base pointer (as the first 2 values 
												; on the stack are the old base pointer and
												; old instruction pointer)
	add bp, 2									; if using .code16gcc add 2 more bytes, as GCC pushes 32bit values on the stack
	mov bx, word [bp]							; get video mode parameter
	mov ax, 4F02h								; command: set SuperVGA video mode
	int 0x10									; call video interrupt
	pop bp										; restore the old base pointer
	ret											; return to callee

enable_a20:										; Enable A20 Gate to access high memory in protected mode (Fast A20)
	in al, 0x92									; read from io port 0x92
	test al, 0x02								; do some test
	out 0x90, al								; write to io port 0x90
	ret											; return to callee

read_e820:										; Read E820 Memory map
	push bp										; save base pointer
	mov bp, sp									; set stack pointer as our base pointer
	add bp, 4									; increment base pointer (as the first 2 values 
												; on the stack are the old base pointer and
												; old instruction pointer + 2 more bytes, as GCC pushes 32bit values on the stack)
	add bp, 2									; if using .code16gcc add 2 more bytes, as GCC pushes 32bit values on the stack
	mov di, word [bp]							; set DI to location where we'll read E820 map (a pointer argument from C)
	add di, 2									; move di to entry table location in E820 map
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
	pop ax										; get entry count
	xor eax, eax								; clear EAX (also status code - failed)
	mov [bp], ax								; no entries on E820 map
	jmp .return									; proceed to funciton exit

.end_ok:
	pop ax										; get entry count
	mov [bp], ax								; save entry count before our memory structure
	mov eax, 1									; status code - ok
	jmp .return									; proceed to funciton exit

.return:
	pop bp										; restore the old base pointer
	ret											; return to callee
