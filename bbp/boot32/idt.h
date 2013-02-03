#ifndef __idt_h
#define __idt_h

#include "common.h"

/**
* Interrupt Descriptor Table (IDT) entry structure
*/
struct idt_entry_struct {
	uint16 base_lo;				// The lower 16 bits of the address to jump to when this interrupt fires.
	uint16 sel;					// Kernel segment selector.
	uint8  always0;				// This must always be zero.
	uint8  flags;				// More flags. See documentation.
	uint16 base_hi;				// The upper 16 bits of the address to jump to.
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
	uint32 base;				// The address of the first element in our idt_entry_t array.
} __PACKED;
/**
* Interrupt Descriptor Table (IDT) pointer
*/
typedef struct idt_ptr_struct idt_ptr_t;

/**
* Set IDT pointer
* @see descriptors.asm
* @param idt_ptr - an address of IDT pointer structure in memory
* @return void
*/
extern void idt_set(uint32 idt_ptr);

#endif
