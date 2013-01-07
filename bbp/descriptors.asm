; This file contains Global Descriptor Table and Interrupt Descriptor
; Table setup routines that are exported to C

[section .text]

[global gdt_set]								; Export gdt_set to C
[global idt_set]								; Export idt_set to C

gdt_set:										; prototype: void gdt_set(uint32 gdt_ptr)
    mov eax, [esp + 4]							; get the pointer to the GDT, passed as a parameter.
    lgdt [eax]									; load the new GDT

	mov ax, 0x0010								; selector 0x10 - data descriptor
	mov ds, ax									; set Data Segment
	mov es, ax									; set Extra Segment
	mov fs, ax									; set Data2 Segment
	mov gs, ax									; set Data3 Segment
	mov ss, ax									; set Stack Segment
    jmp 0x08:.end								; do the magic jump to finalize GDT setup
.end:
    ret											; return to C

idt_set:										; prototype: void idt_set(uint32 idt_ptr)
    mov eax, [esp+4]							; get the pointer to the IDT, passed as a parameter. 
	lidt [eax]									; load the IDT
    ret											; return to C
