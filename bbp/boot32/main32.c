#include "common.h"
#include "gdt.h"
#include "idt.h"
#include "interrupts.h"
#include "screen.h"
#include "memory.h"
#include "string.h"
#include "io.h"
#include "acpi.h"

// Defined in boot16.asm .data section
extern gdt_entry_t gdt[3];
extern gdt_ptr_t gdt_ptr;
extern idt_entry_t idt[256];
extern idt_ptr_t idt_ptr;

// Teletype screen coordinates
uint32 sx = 0;
uint32 sy = 0;

typedef struct {
	uint16 entry_size;	// if 24, then it has attributes
	uint64 base;
	uint64 length;
	uint32 type;
	uint32 attributes; // ACPI 3.0 only
} __PACKED e820_entry_t;

/**
* Initialize all the interrupts
* @return void
*/
static void interrupt_init();
/**
* Set an entry in Interrupt Descriptor Table (IDT)
* @param num - interrupt number
* @param base - function pointer (function's address in memory)
* @param sel - code selector from Global Descriptor Table (GDT)
* @param flags - interrupt flags
* @return void
*/
static void idt_set_entry(uint8 num, uint32 base, uint16 sel, uint8 flags);

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
	"Page gault",
	"",
	"FPU Exception",
	"Alignament check exception",
	"Machine check exception"
};

/**
* Main entry point of C code (called from boot.asm)
* @return void
*/
void main32(){
	uint16 e820_count = *((uint16 *)0x0800);
	e820_entry_t *e820_table = (e820_entry_t *)0x0802;

	screen_clear(0x07);
	screen_print_str("BBP is working fine!", 0x05, sx, sy++);
	
	screen_print_str("E820 Memory Map:", 0x05, sx, sy++);
	while (e820_count--){
		if (e820_table->length > 0){
			screen_print_int((uint32)e820_table->type, 0x05, sx, sy++);
			screen_print_int((uint32)e820_table->base, 0x07, sx, sy++);
			screen_print_int((uint32)e820_table->length, 0x07, sx, sy++);
		}
		e820_table = (e820_entry_t *)(((uint32)e820_table) + e820_table->entry_size + 2);
	}

	interrupt_init();

	// Test interrupts

	//uint32 a = 12;
	//uint32 b = 0;
	//uint32 c = a / b; // Invoke int 0x00

	//asm volatile("int $0x3"); // Invoke "breakpoint"
	
	// Test ACPI

	/*
	RSDP_t *rsdp = acpi_find();
	screen_print_int((uint32)rsdp, 0x07, sx, sy++);
	
	if (rsdp != null){
		SDTHeader_t *th;
		char str[5] = "";
		uint32 i;
		uint32 count;
		
		if (rsdp->revision == 0){
			// ACPI revision 1.0
			RSDT_t *rsdt = (RSDT_t *)rsdp->RSDT_address;
			uint32 ptr;
			count = (rsdt->h.length - sizeof(SDTHeader_t)) / 4;
			for (i = 0; i < count; i ++){
				// Get an address of table pointer array
				ptr = (uint32)&rsdt->ptr;
				// Move on to entry i (32bits = 4 bytes) in table pointer array
				ptr += (i * 4);
				// Get the pointer of table in table pointer array
				th = (SDTHeader_t *)(*((uint32 *)ptr));
				mem_set(0, (uint8 *)str, 5);
				mem_copy((uint8 *)th->signature, (uint8 *)str, 4);
				screen_print_str(str, 0x07, 0, sy++);
			}
		} else {
			// ACPI revision 2.0+
			XSDT_t *xsdt = (XSDT_t *)((uint32)rsdp->XSDT_address);
			uint32 ptr;
			count = (xsdt->h.length - sizeof(SDTHeader_t)) / 8;
			for (i = 0; i < count; i ++){
				// Get an address of table pointer array
				ptr = (uint32)&xsdt->ptr;
				// Move on to entry i (64bits = 8 bytes) in table pointer array
				ptr += (i * 8);
				// Get the pointer of table in table pointer array
				th = (SDTHeader_t *)(*((uint32 *)ptr));
				mem_set(0, (uint8 *)str, 5);
				mem_copy((uint8 *)th->signature, (uint8 *)str, 4);
				screen_print_str(str, 0x07, 0, sy++);
			}
		}

		char apic[4] = {'A', 'P', 'I', 'C'};
		MADT_t *madt = (MADT_t *)acpi_table(apic);
		screen_print_int((uint32)madt, 0x07, sx, sy++);
		if (madt != null){
			screen_print_str("found MADT", 0x07, sx, sy++);
		}

		char facp[4] = {'F', 'A', 'C', 'P'};
		FADT_t *fadt = (FADT_t *)acpi_table(facp);
		screen_print_int((uint32)fadt, 0x07, sx, sy++);
		if (fadt != null){
			screen_print_str("found FADT", 0x07, sx, sy++);
			DSDT_t *dsdt = fadt->dsdt;
			screen_print_int((uint32)dsdt, 0x07, sx, sy++);
			screen_print_str("found DSDT", 0x07, sx, sy++);
			FACS_t *facs = fadt->firmware_ctrl;
			screen_print_int((uint32)facs, 0x07, sx, sy++);
			screen_print_str("found FACS", 0x07, sx, sy++);
		}

		char ssdt_sig[4] = {'S', 'S', 'D', 'T'};
		SSDT_t *ssdt = (SSDT_t *)acpi_table(ssdt_sig);
		screen_print_int((uint32)ssdt, 0x07, sx, sy++);
		if (ssdt != null){
			screen_print_str("found SSDT", 0x07, sx, sy++);
		}
	}
	*/
	
	// Infinite loop
	while(true){}
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

	idt_set_entry( 0, (uint32)isr0 , 0x08, 0x8E);	// Division by zero exception
	idt_set_entry( 1, (uint32)isr1 , 0x08, 0x8E);	// Debug exception
	idt_set_entry( 2, (uint32)isr2 , 0x08, 0x8E);	// Non maskable (external) interrupt
	idt_set_entry( 3, (uint32)isr3 , 0x08, 0x8E);	// Breakpoint exception
	idt_set_entry( 4, (uint32)isr4 , 0x08, 0x8E);	// INTO instruction overflow exception
	idt_set_entry( 5, (uint32)isr5 , 0x08, 0x8E);	// Out of bounds exception (BOUND instruction)
	idt_set_entry( 6, (uint32)isr6 , 0x08, 0x8E);	// Invalid opcode exception
	idt_set_entry( 7, (uint32)isr7 , 0x08, 0x8E);	// No coprocessor exception
	idt_set_entry( 8, (uint32)isr8 , 0x08, 0x8E);	// Double fault (pushes an error code)
	idt_set_entry( 9, (uint32)isr9 , 0x08, 0x8E);	// Coprocessor segment overrun
	idt_set_entry(10, (uint32)isr10, 0x08, 0x8E);	// Bad TSS (pushes an error code)
	idt_set_entry(11, (uint32)isr11, 0x08, 0x8E);	// Segment not present (pushes an error code)
	idt_set_entry(12, (uint32)isr12, 0x08, 0x8E);	// Stack fault (pushes an error code)
	idt_set_entry(13, (uint32)isr13, 0x08, 0x8E);	// General protection fault (pushes an error code)
	idt_set_entry(14, (uint32)isr14, 0x08, 0x8E);	// Page fault (pushes an error code)
	idt_set_entry(15, (uint32)isr15, 0x08, 0x8E);	// Reserved
	idt_set_entry(16, (uint32)isr16, 0x08, 0x8E);	// FPU exception
	idt_set_entry(17, (uint32)isr17, 0x08, 0x8E);	// Alignment check exception
	idt_set_entry(18, (uint32)isr18, 0x08, 0x8E);	// Machine check exception
	idt_set_entry(19, (uint32)isr19, 0x08, 0x8E);	// Reserved
	idt_set_entry(20, (uint32)isr20, 0x08, 0x8E);	// Reserved
	idt_set_entry(21, (uint32)isr21, 0x08, 0x8E);	// Reserved
	idt_set_entry(22, (uint32)isr22, 0x08, 0x8E);	// Reserved
	idt_set_entry(23, (uint32)isr23, 0x08, 0x8E);	// Reserved
	idt_set_entry(24, (uint32)isr24, 0x08, 0x8E);	// Reserved
	idt_set_entry(25, (uint32)isr25, 0x08, 0x8E);	// Reserved
	idt_set_entry(26, (uint32)isr26, 0x08, 0x8E);	// Reserved
	idt_set_entry(27, (uint32)isr27, 0x08, 0x8E);	// Reserved
	idt_set_entry(28, (uint32)isr28, 0x08, 0x8E);	// Reserved
	idt_set_entry(29, (uint32)isr29, 0x08, 0x8E);	// Reserved
	idt_set_entry(30, (uint32)isr30, 0x08, 0x8E);	// Reserved
	idt_set_entry(31, (uint32)isr31, 0x08, 0x8E);	// Reserved
	idt_set_entry(32, (uint32)irq0, 0x08, 0x8E);	// IRQ0 - Programmable Interrupt Timer Interrupt
	idt_set_entry(33, (uint32)irq1, 0x08, 0x8E);	// IRQ1 - Keyboard Interrupt
	idt_set_entry(34, (uint32)irq2, 0x08, 0x8E);	// IRQ2 - Cascade (used internally by the two PICs. never raised)
	idt_set_entry(35, (uint32)irq3, 0x08, 0x8E);	// IRQ3 - COM2 (if enabled)
	idt_set_entry(36, (uint32)irq4, 0x08, 0x8E);	// IRQ4 - COM1 (if enabled)
	idt_set_entry(37, (uint32)irq5, 0x08, 0x8E);	// IRQ5 - LPT2 (if enabled)
	idt_set_entry(38, (uint32)irq6, 0x08, 0x8E);	// IRQ6 - Floppy Disk
	idt_set_entry(39, (uint32)irq7, 0x08, 0x8E);	// IRQ7 - LPT1 / Unreliable "spurious" interrupt (usually)
	idt_set_entry(40, (uint32)irq8, 0x08, 0x8E);	// IRQ8 - CMOS real-time clock (if enabled)
	idt_set_entry(41, (uint32)irq9, 0x08, 0x8E);	// IRQ9 - Free for peripherals / legacy SCSI / NIC
	idt_set_entry(42, (uint32)irq10, 0x08, 0x8E);	// IRQ10 - Free for peripherals / SCSI / NIC
	idt_set_entry(43, (uint32)irq11, 0x08, 0x8E);	// IRQ11 - Free for peripherals / SCSI / NIC
	idt_set_entry(44, (uint32)irq12, 0x08, 0x8E);	// IRQ12 - PS2 Mouse
	idt_set_entry(45, (uint32)irq13, 0x08, 0x8E);	// IRQ13 - FPU / Coprocessor / Inter-processor
	idt_set_entry(46, (uint32)irq14, 0x08, 0x8E);	// IRQ14 - Primary ATA Hard Disk
	idt_set_entry(47, (uint32)irq15, 0x08, 0x8E);	// IRQ15 - Secondary ATA Hard Disk

	idt_set((uint32)&idt_ptr);
}

static void idt_set_entry(uint8 num, uint32 base, uint16 sel, uint8 flags){
	idt[num].base_lo = base & 0xFFFF;
	idt[num].base_hi = (base >> 16) & 0xFFFF;
	idt[num].sel     = sel;
	idt[num].always0 = 0;
	// We must uncomment the OR below when we get to using user-mode.
	// It sets the interrupt gate's privilege level to 3.
	idt[num].flags   = flags /* | 0x60 */;
}

void isr_handler(registers_t regs){
	char msg[80] = "Interrupt ";
	if (regs.int_no < 19){
		str_copy(ints[regs.int_no], msg, 40);
		if (regs.int_no == 0){
			regs.eip++; // it's ok to divide by zero - move to next instruction :P
		}
	} else {
		char *m = &msg[10];
		uint32 len = int_to_str(regs.int_no, m, 20);
		msg[10 + len] = 0x20;
		m = &msg[11 + len];
		int_to_str(regs.err_code, m, 20);
	}
	screen_print_str(msg, 0x08, sx, sy++);
	if (sy >= 25){
		sy = 0;
	}
}

void irq_handler(registers_t regs){
	screen_print_str("IRQ ", 0x08, sx, sy);
	screen_print_int(regs.err_code, 0x05, sx + 4, sy++);
	if (sy >= 25){
		sy = 0;
	}
}
