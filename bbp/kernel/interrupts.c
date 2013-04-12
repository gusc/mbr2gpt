/*

Helper functions for operations interrupts
==========================================

This contains imports from ASM stub subroutines that catch all the neccessary
exception interrupts and calls a C function interrupt_handler().
It also contains LIDT wrapper function and C functions to handle interrupts.

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

#include "interrupts.h"
#include "lib.h"
#include "io.h"
#include "../config.h"
#if DEBUG == 1
	#include "debug_print.h"
#endif

/**
* Exception names
*/
static char *ints[] = {
  "Division by zero",
  "Debug exception",
  "NMI interrupt",
  "Breakpoint",
  "INTO overflow",
  "BOUND exception",
  "Invalid opcode",
  "No FPU",
  "Double Fault!",
  "FPU segment overrun",
  "Bad TSS",
  "Segment not present",
  "Stack fault",
  "GPF",
  "Page fault",
  "",
  "FPU Exception",
  "Alignament check exception",
  "Machine check exception"
};
/**
* Interrupt Descriptor Table
*/
idt_entry_t idt[256];
/**
* Interrupt Descriptor Table pointer
*/
idt_ptr_t idt_ptr;

static void idt_set_entry(uint8 num, uint64 addr, uint16 flags){
	idt[num].offset_lo = (uint16)(addr & 0xFFFF);
	idt[num].offset_hi = (uint16)((addr >> 16) & 0xFFFF);
	idt[num].offset_64 = (uint32)((addr >> 32) & 0xFFFFFFFFF);
	idt[num].segment = 0x08; // Allways a code selector
	idt[num].flags.raw = flags;
	idt[num].reserved = 0; // Zero out
}

void interrupt_init(){
	mem_fill((uint8 *)&idt, sizeof(idt_entry_t) * 256, 0);

	// Remap the IRQ table.
	outb(0x20, 0x11); // Initialize master PIC
	outb(0xA0, 0x11); // Initialize slave PIC
	outb(0x21, 0x20); // Master PIC vector offset (IRQ0 target interrupt number)
	outb(0xA1, 0x28); // Slave PIC vector offset (IRQ8 landing interrupt number)
	outb(0x21, 0x04); // Tell Master PIC that Slave PIC is at IRQ2
	outb(0xA1, 0x02); // Tell Slave PIC that it's cascaded to IRQ2
	outb(0x21, 0x01); // Enable 8085 mode (whatever that means)
	outb(0xA1, 0x01); // Enable 8085 mode (whatever that means)
	outb(0x21, 0x0); // Clear masks
	outb(0xA1, 0x0); // Clear masks

	idt_set_entry( 0, (uint64)isr0 , 0x8E00);  // Division by zero exception
	idt_set_entry( 1, (uint64)isr1 , 0x8E00);  // Debug exception
	idt_set_entry( 2, (uint64)isr2 , 0x8E00);  // Non maskable (external) interrupt
	idt_set_entry( 3, (uint64)isr3 , 0x8E00);  // Breakpoint exception
	idt_set_entry( 4, (uint64)isr4 , 0x8E00);  // INTO instruction overflow exception
	idt_set_entry( 5, (uint64)isr5 , 0x8E00);  // Out of bounds exception (BOUND instruction)
	idt_set_entry( 6, (uint64)isr6 , 0x8E00);  // Invalid opcode exception
	idt_set_entry( 7, (uint64)isr7 , 0x8E00);  // No coprocessor exception
	idt_set_entry( 8, (uint64)isr8 , 0x8E00);  // Double fault (pushes an error code)
	idt_set_entry( 9, (uint64)isr9 , 0x8E00);  // Coprocessor segment overrun
	idt_set_entry(10, (uint64)isr10, 0x8E00);  // Bad TSS (pushes an error code)
	idt_set_entry(11, (uint64)isr11, 0x8E00);  // Segment not present (pushes an error code)
	idt_set_entry(12, (uint64)isr12, 0x8E00);  // Stack fault (pushes an error code)
	idt_set_entry(13, (uint64)isr13, 0x8E00);  // General protection fault (pushes an error code)
	idt_set_entry(14, (uint64)isr14, 0x8E00);  // Page fault (pushes an error code)
	idt_set_entry(15, (uint64)isr15, 0x8E00);  // Reserved
	idt_set_entry(16, (uint64)isr16, 0x8E00);  // FPU exception
	idt_set_entry(17, (uint64)isr17, 0x8E00);  // Alignment check exception
	idt_set_entry(18, (uint64)isr18, 0x8E00);  // Machine check exception
	idt_set_entry(19, (uint64)isr19, 0x8E00);  // Reserved
	idt_set_entry(20, (uint64)isr20, 0x8E00);  // Reserved
	idt_set_entry(21, (uint64)isr21, 0x8E00);  // Reserved
	idt_set_entry(22, (uint64)isr22, 0x8E00);  // Reserved
	idt_set_entry(23, (uint64)isr23, 0x8E00);  // Reserved
	idt_set_entry(24, (uint64)isr24, 0x8E00);  // Reserved
	idt_set_entry(25, (uint64)isr25, 0x8E00);  // Reserved
	idt_set_entry(26, (uint64)isr26, 0x8E00);  // Reserved
	idt_set_entry(27, (uint64)isr27, 0x8E00);  // Reserved
	idt_set_entry(28, (uint64)isr28, 0x8E00);  // Reserved
	idt_set_entry(29, (uint64)isr29, 0x8E00);  // Reserved
	idt_set_entry(30, (uint64)isr30, 0x8E00);  // Reserved
	idt_set_entry(31, (uint64)isr31, 0x8E00);  // Reserved

	idt_set_entry(32, (uint64)irq0 , 0x8E00);  // IRQ0 - Programmable Interrupt Timer Interrupt
	idt_set_entry(33, (uint64)irq1 , 0x8E00);  // IRQ1 - Keyboard Interrupt
	idt_set_entry(34, (uint64)irq2 , 0x8E00);  // IRQ2 - Cascade (used internally by the two PICs. never raised)
	idt_set_entry(35, (uint64)irq3 , 0x8E00);  // IRQ3 - COM2 (if enabled)
	idt_set_entry(36, (uint64)irq4 , 0x8E00);  // IRQ4 - COM1 (if enabled)
	idt_set_entry(37, (uint64)irq5 , 0x8E00);  // IRQ5 - LPT2 (if enabled)
	idt_set_entry(38, (uint64)irq6 , 0x8E00);  // IRQ6 - Floppy Disk
	idt_set_entry(39, (uint64)irq7 , 0x8E00);  // IRQ7 - LPT1 / Unreliable "spurious" interrupt (usually)
	idt_set_entry(40, (uint64)irq8 , 0x8E00);  // IRQ8 - CMOS real-time clock (if enabled)
	idt_set_entry(41, (uint64)irq9 , 0x8E00);  // IRQ9 - Free for peripherals / legacy SCSI / NIC
	idt_set_entry(42, (uint64)irq10, 0x8E00);  // IRQ10 - Free for peripherals / SCSI / NIC
	idt_set_entry(43, (uint64)irq11, 0x8E00);  // IRQ11 - Free for peripherals / SCSI / NIC
	idt_set_entry(44, (uint64)irq12, 0x8E00);  // IRQ12 - PS2 Mouse
	idt_set_entry(45, (uint64)irq13, 0x8E00);  // IRQ13 - FPU / Coprocessor / Inter-processor
	idt_set_entry(46, (uint64)irq14, 0x8E00);  // IRQ14 - Primary ATA Hard Disk
	idt_set_entry(47, (uint64)irq15, 0x8E00);  // IRQ15 - Secondary ATA Hard Disk

	idt_ptr.limit = (sizeof(idt_entry_t) * 256) - 1;
	idt_ptr.base = (uint64)&idt;
	idt_set(&idt_ptr);
}

void isr_handler(int_stack_t stack){
#if DEBUG == 1
	if (stack.int_no < 19){
		debug_print(DC_WB, ints[stack.int_no]);
	} else {
		debug_print(DC_WB, "Interrupt: %x", stack.int_no);
	}
#endif
	// Process some exceptions here
	uint64 cr2 = 0;
	switch (stack.int_no){
		case 0: // Division by zero
			//stack.rip++; // it's ok to divide by zero - move to next instruction :P
			break;
		case 13: // General protection fault
#if DEBUG == 1
			debug_print(DC_WRD, "Error: %x", stack.err_code);
#endif
			HANG();
			break;
		case 14: // Page fault			
#if DEBUG == 1
			asm volatile ("mov %%cr2, %0" : "=a"(cr2) :);
			debug_print(DC_WRD, "Error: %x", stack.err_code);
			debug_print(DC_WRD, "Addr: @%x", cr2);
#endif
			HANG();
			// Do something!
			break;
	}
}

void irq_handler(int_stack_t stack){
#if DEBUG == 1
	debug_print(DC_WB, "IRQ %d", stack.err_code);
#endif
}