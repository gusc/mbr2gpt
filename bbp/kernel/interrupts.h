/*

Helper functions for operations interrupts
==========================================

This contains imports from ASM stub subroutines that catch all the neccessary
exception interrupts and calls a C function interrupt_handler().
It also contains LIDT wrapper function.

License (BSD-3)
===============

Copyright (c) 2012, Gusts 'gusC' Kaksis <gusts.kaksis@gmail.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef __interrupts_h
#define __interrupts_h

#include "common.h"

// Remaped IRQ numbers to interrupt numbers
#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

/**
* Register stack passed from assembly
*/
typedef struct {
	uint64 int_no;				// Interrupt number
	uint64 err_code;			// Error code (or IRQ number for IRQs)
	uint64 rip;					// Return instruction pointer
	uint64 cs;					// Code segment
	uint64 rflags;				// RFLAGS
	uint64 rsp;					// Previous stack pointer
	uint64 ss;					// Stack segment
} int_stack_t;
/**
* Interrupt Descriptor Table (IDT) entry structure
*/
struct idt_entry_struct {
	uint16 offset_lo;			// The lower 16 bits of 32bit address to jump to when this interrupt fires
	uint16 segment;				// Kernel segment selector
	union {
		uint16 raw;				// Raw value
		struct {				// IDT flag structure
			uint16 ist		:3;	// Interrupt stack table
			uint16 res1		:5;	// This must be zero
			uint16 type		:4;	// Interrupt gate, trap gate, task gate, etc.
			uint16 res2		:1;	// This must be zero
			uint16 dpl		:2;	// Descriptor privilege level
			uint16 present	:1;	// Present flag
		} s;
	} flags;
	uint16 offset_hi;			// The upper 16 bits of 32bit address to jump to
	uint32 offset_64;			// The upper 32 bits of 64bit address
	uint32 reserved;			// Reserved for 96bit systems :)
} __PACKED;
/**
* Interrupt Descriptor Table (IDT) entry
*/
typedef struct idt_entry_struct idt_entry_t;
/**
* Interrupt Descriptor Table (IDT) pointer structure
*/
struct idt_ptr_struct {
	uint16 limit;
	uint64 base;				// The address of the first element in our idt_entry_t array.
} __PACKED;
/**
* Interrupt Descriptor Table (IDT) pointer
*/
typedef struct idt_ptr_struct idt_ptr_t;
/**
* Initialize interrupt handlers
*/
void interrupt_init();
/**
* Set IDT pointer
* @see interrupts.asm
* @param idt_ptr - an address of IDT pointer structure in memory
* @return void
*/
extern void idt_set(idt_ptr_t *idt_ptr);
/**
* Interrupt Service Routine (ISR) handler
* This will be defined in kernel code
* @param regs - registers pushed on the stack by assembly
* @return void
*/
void isr_handler(int_stack_t regs);
/**
* Interrupt Request (IRQ) handler
* This will be defined in kernel code
* @param regs - registers pushed on the stack by assembly
* @return void
*/
void irq_handler(int_stack_t regs);

// Defined in interrupts.asm (with macros!)
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
// Defined in interrupts.asm (with macros!)
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();


#endif