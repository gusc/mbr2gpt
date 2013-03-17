/*

Helper functions for operations with IDT setup
==============================================

Functions are defined in idt.asm file

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
