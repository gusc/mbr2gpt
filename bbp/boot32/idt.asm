[section .text]
[bits 32]
[global idt_set]								; Export idt_set to C

idt_set:										; prototype: void idt_set(uint32 idt_ptr)
	push ebp									; store current ebp on the stack
	mov ebp, esp								; copy esp to ebp
	mov eax, [ebp + 8]							; get the pointer to the IDT, passed as a parameter. 
	lidt [eax]									; load the IDT
	pop ebp										; restore ebp from stack
	ret											; return to C
