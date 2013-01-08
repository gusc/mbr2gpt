#include "kmain.h"
#include "screen.h"
#include "memory.h"

idt_entry_t idt[256];
idt_ptr_t   idt_ptr;

static uint32 screen_line = 0;

static void interrupt_init();
static void idt_set_entry(uint8 num, uint32 base, uint16 sel, uint8 flags);

void kmain(){
	screen_clear(0x07);
	screen_print("BBP is working fine!", 0x05, screen_line++);

	interrupt_init();
}

static void interrupt_init(){
	idt_ptr.limit = sizeof(idt_entry_t) * 256 -1;
    idt_ptr.base  = (uint32)&idt;

    mem_set(0, (uint8 *)&idt, sizeof(idt_entry_t)*256);

    idt_set_entry( 0, (uint32)isr0 , 0x08, 0x8E);
    idt_set_entry( 1, (uint32)isr1 , 0x08, 0x8E);
    idt_set_entry( 2, (uint32)isr2 , 0x08, 0x8E);
    idt_set_entry( 3, (uint32)isr3 , 0x08, 0x8E);
    idt_set_entry( 4, (uint32)isr4 , 0x08, 0x8E);
    idt_set_entry( 5, (uint32)isr5 , 0x08, 0x8E);
    idt_set_entry( 6, (uint32)isr6 , 0x08, 0x8E);
    idt_set_entry( 7, (uint32)isr7 , 0x08, 0x8E);
    idt_set_entry( 8, (uint32)isr8 , 0x08, 0x8E);
    idt_set_entry( 9, (uint32)isr9 , 0x08, 0x8E);
    idt_set_entry(10, (uint32)isr10, 0x08, 0x8E);
    idt_set_entry(11, (uint32)isr11, 0x08, 0x8E);
    idt_set_entry(12, (uint32)isr12, 0x08, 0x8E);
    idt_set_entry(13, (uint32)isr13, 0x08, 0x8E);
    idt_set_entry(14, (uint32)isr14, 0x08, 0x8E);
    idt_set_entry(15, (uint32)isr15, 0x08, 0x8E);
    idt_set_entry(16, (uint32)isr16, 0x08, 0x8E);
    idt_set_entry(17, (uint32)isr17, 0x08, 0x8E);
    idt_set_entry(18, (uint32)isr18, 0x08, 0x8E);
    idt_set_entry(19, (uint32)isr19, 0x08, 0x8E);
    idt_set_entry(20, (uint32)isr20, 0x08, 0x8E);
    idt_set_entry(21, (uint32)isr21, 0x08, 0x8E);
    idt_set_entry(22, (uint32)isr22, 0x08, 0x8E);
    idt_set_entry(23, (uint32)isr23, 0x08, 0x8E);
    idt_set_entry(24, (uint32)isr24, 0x08, 0x8E);
    idt_set_entry(25, (uint32)isr25, 0x08, 0x8E);
    idt_set_entry(26, (uint32)isr26, 0x08, 0x8E);
    idt_set_entry(27, (uint32)isr27, 0x08, 0x8E);
    idt_set_entry(28, (uint32)isr28, 0x08, 0x8E);
    idt_set_entry(29, (uint32)isr29, 0x08, 0x8E);
    idt_set_entry(30, (uint32)isr30, 0x08, 0x8E);
    idt_set_entry(31, (uint32)isr31, 0x08, 0x8E);

    idt_set((uint32)&idt_ptr);
}

static void idt_set_entry(uint8 num, uint32 base, uint16 sel, uint8 flags)
{
    idt[num].base_lo = base & 0xFFFF;
    idt[num].base_hi = (base >> 16) & 0xFFFF;

    idt[num].sel     = sel;
    idt[num].always0 = 0;
    // We must uncomment the OR below when we get to using user-mode.
    // It sets the interrupt gate's privilege level to 3.
    idt[num].flags   = flags /* | 0x60 */;
}

void interrupt_handler(registers_t regs){
	screen_print("Interrupt", 0x08, screen_line++);
}
