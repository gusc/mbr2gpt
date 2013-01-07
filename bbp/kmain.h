#ifndef __kmain_h
#define __kmain_h

#include "common.h"
#include "descriptors.h"
#include "interrupts.h"

extern void gdt_set(uint32 gdt_ptr);
extern void idt_set(uint32 idt_ptr);

void interrupt_handler(registers_t regs);

void k_clear_screen();
unsigned int k_printf(char *message, unsigned int line);

#endif
