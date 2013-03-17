/*

Protected Mode initialization
=============================

This file contains initialization code protected mode (preparation to switch to 
long mode).

It does:
	* page setup and initialization

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

#include "main32.h"
#include "../config.h"

/**
* Current memory map
*/
//static e820map_t *mem_map = (e820map_t *)E820_LOC;
/**
* Page table structures
*/
// @ D000
static pm_t pml4[512] __ALIGN(4096); // a.k.a. PML4T
// @ E000
static pm_t pml3[512] __ALIGN(4096); // a.k.a. PTD (page directory table)
// @ F000
static pm_t pml2[512] __ALIGN(4096); // a.k.a. PD (page directory)
// @ 10000
static pm_t pml1[8][512] __ALIGN(4096); // a.k.a. PT (page table), and it will map 0-16Mb (each PT can map 2Mb, thus we need 8 tables)
/**
* PML4 pointer (to be passed over to CR3)
* @see boot16.asm
*/
extern uint32 pml4_ptr;
/**
* Set value in memory
* @param val - the value to set
* @param dest - destination address (pointer to destination buffer)
* @param len - length to fill
*/
static void mem_set(uint8 val, uint8 *dest, uint16 len){
	while(len--){
		*dest++ = val;
	}
}
/**
* Setup PML4 pages to enter Long Mode
*/
static void setup_pages(){
	/**
	* Page mapping structures
	*/
	uint32 t;
	uint32 i;
	uint64 ptr;
	// Clear memory region where the page tables will reside
	mem_set(0, (uint8 *)&pml4, sizeof(pm_t) * 512);
	mem_set(0, (uint8 *)&pml3, sizeof(pm_t) * 512);
	mem_set(0, (uint8 *)&pml2, sizeof(pm_t) * 512);
	mem_set(0, (uint8 *)&pml1, sizeof(pm_t) * 8 * 512);
	// Each PT is used for 4Mb, and we'll identity-map all the first 16Mb for kernel
	for (t = 0; t < 8; t ++){
		// Create the identity map (virtual address = physical address)
		for (i = 0; i < 512; i ++){
			pml1[t][i].val = ((t * 2097152) + (i * 4096)) & PAGE_MASK;
			pml1[t][i].s.present = 1;
			pml1[t][i].s.writable = 1;
		}
		// Point PML2 entries to PML1 tables
		ptr = ((uint64)&pml1) + (t * 4096);
		pml2[t].val = ptr & PAGE_MASK;
		pml2[t].s.present = 1;
		pml2[t].s.writable = 1;
		
	}
	// Point first PML3 entry to PML2 table
	ptr = (uint64)&pml2;
	pml3[0].val = ptr & PAGE_MASK;
	pml3[0].s.present = 1;
	pml3[0].s.writable = 1;
	// Point first PML4 entry to PML3 table
	ptr = (uint64)&pml3;
	pml4[0].val = ptr & PAGE_MASK;
	pml4[0].s.present = 1;
	pml4[0].s.writable = 1;
	// Set PML4 pointer address
	pml4_ptr = (uint32)&pml4;
}

/**
* Initialize Protected Mode
*/
void main32(){
	// Setup Page tables
	setup_pages();
}
