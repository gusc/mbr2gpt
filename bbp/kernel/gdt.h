/*

Helper functions for operations with GDT setup
==============================================

Functions are defined in gdt.asm file

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
