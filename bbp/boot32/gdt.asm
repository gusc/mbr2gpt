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
