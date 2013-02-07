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
} __PACKED;
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
} __PACKED;
/**
* Global Descriptor Table (GDT) pointer
*/
typedef struct gdt_ptr_struct gdt_ptr_t;

/**
* Set GDT pointer and reload protected mode
* @see descriptors.asm
* @param gdt_ptr - an address of GDT pointer structure in memory
* @return void
*/
extern void gdt_set(uint32 gdt_ptr);

#endif