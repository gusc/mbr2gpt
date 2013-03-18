/*

Kernel entry point
==================

This is where the fun part begins

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

#include "kmain.h"
#include "screen.h"
#include "memory.h"
#include "string.h"
#include "acpi.h"
#include "io.h"
#include "interrupts.h"

// Global cursor position
uint8 sx = 0;
uint8 sy = 0;
/**
* Interrupt Descriptor Table
*/
idt_entry_t idt[256];
/**
* Interrupt Descriptor Table pointer
*/
idt_ptr_t idt_ptr;
/**
* Page table structures
*/
//static pm_t pml4[512] __ALIGN(4096); // a.k.a. PML4T
//static pm_t pml3[512] __ALIGN(4096); // a.k.a. PTD (page directory table)
//static pm_t pml2[512] __ALIGN(4096); // a.k.a. PD (page directory)
//static pm_t pml1[8][512] __ALIGN(4096); // a.k.a. PT (page table), and it will map 0-16Mb (each PT can map 2Mb, thus we need 8 tables)
/**
* Initialize all the interrupts
* @return void
*/
static void interrupt_init();
/**
* Initialize ACPI tables
* @return void
*/
static void acpi_init();

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
* Kernel entry point
*/
void kmain(){
	// Initialize interrupts
	interrupt_init();
	// Show something on the screen
	screen_clear(0x07);
	screen_print_str("Long mode", 0x05, sx, sy++);
	// Initialize ACPI
	acpi_init();
	// Test interrupt exception: division by zero
	uint32 a = 1;
	uint32 b = 0;
	uint32 c = a / b;
	// Infinite loop
	while(true){}
}

static void acpi_init(){
	RSDP_t *rsdp = acpi_find();
	screen_print_int((uint64)rsdp, 0x07, sx, sy++);
  
	if (rsdp != null){
		SDTHeader_t *th;
		char sign[5] = "";
		uint32 i;
		uint32 count;
		screen_print_int((uint32)rsdp->revision, 0x07, sx, sy++);
		if (rsdp->revision == 0){
			// ACPI revision 1.0
			RSDT_t *rsdt = (RSDT_t *)((uint64)rsdp->RSDT_address);
			uint64 ptr;
			count = (rsdt->h.length - sizeof(SDTHeader_t)) / 4;
			for (i = 0; i < count; i ++){
				// Get an address of table pointer array
				ptr = (uint64)&rsdt->ptr;
				// Move on to entry i (32bits = 4 bytes) in table pointer array
				ptr += (i * 4);
				// Get the pointer of table in table pointer array
				th = (SDTHeader_t *)((uint64)(*((uint32 *)ptr)));
				mem_set(0, (uint8 *)sign, 5);
				mem_copy((uint8 *)th->signature, (uint8 *)sign, 4);
				screen_print_str(sign, 0x07, 0, sy++);
			}
		} else {
			// ACPI revision 2.0+
			screen_print_int((uint64)rsdp->XSDT_address, 0x07, sx, sy++);
			XSDT_t *xsdt = (XSDT_t *)(rsdp->XSDT_address);
			uint64 ptr;
			count = (xsdt->h.length - sizeof(SDTHeader_t)) / 8;
			for (i = 0; i < count; i ++){
				// Get an address of table pointer array
				ptr = (uint64)&xsdt->ptr;
				// Move on to entry i (64bits = 8 bytes) in table pointer array
				ptr += (i * 8);
				// Get the pointer of table in table pointer array
				th = (SDTHeader_t *)(*((uint64 *)ptr));
				mem_set(0, (uint8 *)sign, 5);
				mem_copy((uint8 *)th->signature, (uint8 *)sign, 4);
				screen_print_str(sign, 0x07, 0, sy++);
			}
		}
		
		char apic[4] = {'A', 'P', 'I', 'C'};
		MADT_t *madt = (MADT_t *)acpi_table(apic);
		screen_print_int((uint64)madt, 0x07, sx, sy++);
		if (madt != null){
			screen_print_str("found MADT", 0x07, sx, sy++);
		}

		char facp[4] = {'F', 'A', 'C', 'P'};
		FADT_t *fadt = (FADT_t *)acpi_table(facp);
		screen_print_int((uint64)fadt, 0x07, sx, sy++);
		if (fadt != null){
			screen_print_str("found FADT", 0x07, sx, sy++);
			DSDT_t *dsdt = fadt->dsdt;
			screen_print_int((uint64)dsdt, 0x07, sx, sy++);
			screen_print_str("found DSDT", 0x07, sx, sy++);
			FACS_t *facs = fadt->firmware_ctrl;
			screen_print_int((uint64)facs, 0x07, sx, sy++);
			screen_print_str("found FACS", 0x07, sx, sy++);
		}

		char ssdt_sig[4] = {'S', 'S', 'D', 'T'};
		SSDT_t *ssdt = (SSDT_t *)acpi_table(ssdt_sig);
		screen_print_int((uint64)ssdt, 0x07, sx, sy++);
		if (ssdt != null){
			screen_print_str("found SSDT", 0x07, sx, sy++);
		}
		
	}
}

static void idt_set_entry(uint8 num, uint64 addr, uint16 flags){
	idt[num].offset_lo = (uint16)(addr & 0xFFFF);
	idt[num].offset_hi = (uint16)((addr >> 16) & 0xFFFF);
	idt[num].offset_64 = (uint32)((addr >> 32) & 0xFFFFFFFFF);
	idt[num].segment = 0x08; // Allways a code selector
	idt[num].flags.raw = flags;
	idt[num].reserved = 0; // Zero out
}

static void interrupt_init(){
	mem_set(0, (uint8 *)&idt, sizeof(idt_entry_t) * 256);

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
	char msg[80] = "Interrupt ";
	if (stack.int_no < 19){
		str_copy(ints[stack.int_no], msg, 40);
		switch (stack.int_no){
			case 0: // Division by zero
				stack.rip++; // it's ok to divide by zero - move to next instruction :P
				break;
			case 14: // Page fault
				// Do something!
				break;
		}
	} else {
		char *m = &msg[10];
		uint32 len = int_to_str(stack.int_no, m, 20);
		msg[10 + len] = 0x20;
		m = &msg[11 + len];
		int_to_str(stack.err_code, m, 20);
	}
	screen_print_str(msg, 0x08, sx, sy++);
	if (sy >= 25){
		sy = 0;
	}
}

void irq_handler(int_stack_t stack){
	screen_print_str("IRQ ", 0x08, sx, sy);
	screen_print_int(stack.err_code, 0x05, sx + 4, sy++);
	if (sy >= 25){
		sy = 0;
	}
}
