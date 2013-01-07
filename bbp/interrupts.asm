; This file contains Interrupt wrapper subroutine that calls a C function interrupt_handler()

[section .text]

[extern interrupt_handler]						; Import interrupt_handler from C

; Macro to create an intterupt service routine for interrupts that do not pass error codes 
%macro INT_NO_ERR 1
  global isr%1
  isr%1:
    cli											; disable interrupts
    push byte 0									; set error code to 0
    push byte %1								; set interrupt number
    jmp isr_handler								; jump to our local handler
%endmacro

; Macro to create an interrupt service routine for interrupts that DO pass an error code
%macro INT_HAS_ERR 1
  global isr%1
  isr%1:
    cli											; disable interrupts
    push byte %1								; set interrupt number
    jmp isr_handler								; jump to our local handler
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
INT_HAS_ERR 8
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

isr_handler:									; Local interrupt handler
    pusha										; push all the register on the stack edi,esi,ebp,esp,ebx,edx,ecx,eax
	mov ax, ds									; store ds in lower 16-bits of eax
    push eax									; save the data segment descriptor
	mov ax, 0x10								; load the kernel data segment descriptor
    mov ds, ax									; set Data Segment
	mov es, ax									; set Extra Segment
	mov fs, ax									; set Data2 Segment
	mov gs, ax									; set Data3 Segment

    call interrupt_handler						; call our interrupt_handler() C function 

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
