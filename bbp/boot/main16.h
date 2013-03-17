/*

Real Mode initialization
=============================

This file contains initialization code real mode (preparation to switch to 
protected mode).

It does:
	* video mode switch
	* memory mapping

License (BSD-3)
===============

Copyright (c) 2013, Gusts 'gusC' Kaksis <gusts.kaksis@gmail.com>
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

#ifndef __main16_h
#define __main16_h

#include "common16.h"

/**
* Memory type codes for E820
*/
enum eMemType {
	kMemOk = 1,			// Normal memory - usable
	kMemReserved,		// Reserved memory - unusable
	kMemACPIReclaim,	// ACPI reclaimable memory - might be usable after ACPI is taken care of
	kMemACPI,			// ACPI NVS memory - unusable
	kMemBad				// Bad memory - unsuable
};
/**
* E820 memory map entry structure
*/
struct e820entry_struct {
	uint16 entry_size;	// if 24, then it has attributes
	uint64 base;
	uint64 length;
	uint32 type;
	uint32 attributes;	// ACPI 3.0 only
} __PACKED;
typedef struct e820entry_struct e820entry_t;
/**
* E820 memory map structure
*/
struct e820map_struct {
	uint16 size;
	e820entry_t entries[];
} __PACKED;
typedef struct e820map_struct e820map_t;

#endif  /* __main16_h */
