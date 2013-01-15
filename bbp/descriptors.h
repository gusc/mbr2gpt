/**
* @file descriptors.h
* Helper functions for operations with IDT and GDT setup
* @see descriptors.asm - for implementation
*/
#ifndef __descriptors_h
#define __descriptors_h

#include "common.h"

/**
* Global Descriptor Table (GDT) entry structure
*/
struct gdt_entry_struct {
	uint16 limit_low;			// The lower 16 bits of the limit.
	uint16 base_low;			// The lower 16 bits of the base.
	uint8  base_middle;			// The next 8 bits of the base.
	uint8  access;				// Access flags, determine what ring this segment can be used in.
	uint8  granularity;
	uint8  base_high;			// The last 8 bits of the base.
} __attribute__((packed));
/**
* Global Descriptor Table (GDT) entry
*/
typedef struct gdt_entry_struct gdt_entry_t;

/**
* GDT Pointer structure
*/
struct gdt_ptr_struct {
	uint16 limit;				// The upper 16 bits of all selector limits.
	uint32 base;				// The address of the first gdt_entry_t struct.
} __attribute__((packed));
/**
* Global Descriptor Table (GDT) pointer
*/
typedef struct gdt_ptr_struct gdt_ptr_t;

/**
* Interrupt Descriptor Table (IDT) entry structure
*/
struct idt_entry_struct {
	uint16 base_lo;				// The lower 16 bits of the address to jump to when this interrupt fires.
	uint16 sel;					// Kernel segment selector.
	uint8  always0;				// This must always be zero.
	uint8  flags;				// More flags. See documentation.
	uint16 base_hi;				// The upper 16 bits of the address to jump to.
} __attribute__((packed));
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
} __attribute__((packed));
/**
* Interrupt Descriptor Table (IDT) pointer
*/
typedef struct idt_ptr_struct idt_ptr_t;

/**
* Set GDT pointer and reload protected mode
* @see descriptors.asm
* @param gdt_ptr - an address of GDT pointer structure in memory
* @return void
*/
extern void gdt_set(uint32 gdt_ptr);
/**
* Set IDT pointer
* @see descriptors.asm
* @param idt_ptr - an address of IDT pointer structure in memory
* @return void
*/
extern void idt_set(uint32 idt_ptr);

#endif
