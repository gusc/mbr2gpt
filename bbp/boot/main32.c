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
* PML4 pointer (to be passed over to CR3)
* @see boot.asm
*/
extern uint32 pml4_ptr32;

/**
* Clear memory region
* @param dest - destination address (pointer to destination buffer)
* @param len - length to clear
*/
static void mem_clear(uint8 *dest, uint32 len){
	while(len--){
		*dest++ = 0;
	}
}
/**
* Setup PML4 pages to enter Long Mode
* @param ammount - ammount of memory to map
*/
static void setup_pages(uint64 ammount){
	uint64 p;
	uint64 t;
	uint64 d;
	uint64 dr;
	uint64 ptr;
	
	// Single page (PML1 entry) holds 4KB of RAM
	uint64 page_count = ammount / PAGE_SIZE;
	if (ammount % PAGE_SIZE > 0){
		page_count ++;
	}
	// Single table (PML2 entry) holds 2MB of RAM
	uint64 table_count = page_count / 512;
	if (page_count % 512 > 0){
		table_count ++;
	}
	// Single directory (PML3 entry, directory table pointer) holds 1GB of RAM
	uint64 directory_count = table_count / 512;
	if (table_count % 512 > 0){
		directory_count ++;
	}
	// Single drawer (PML4 entry) holds 512GB of RAM
	uint64 drawer_count = directory_count / 512;
	if (directory_count % 512 > 0){
		drawer_count ++;
	}
	
	// Position the page table structures in memory

	// Located at 0x00100000 (1MB mark, see config.h)
	// a.k.a. PML4T (512GB per entry = 256TB total, this is a page cabinet)
	// Holds 512 entries, only 1st is active - enough to map 512GB
	pm_t *pml4 = (pm_t*)PT_LOC; 
	// Located at PML4 + (8 * 512)
	// a.k.a. PDPT (page directory pointer table, 1GB per entry, let's call this a page drawer)
	// Holds 512 entries, each entry maps up to 1GB, table = 512GB
	pm_t *pml3 = (pm_t*)(((uint32)pml4) + (sizeof(pm_t) * 512));
	// Located at PML3 + (8 * 512 * drawer_count)
	// a.k.a. PD (page directory, 2MB per entry)
	// Holds 512 entries * directory_count, each entry maps up to 2MB, table = 1GB
	pm_t *pml2 = (pm_t*)(((uint32)pml3) + (sizeof(pm_t) * 512 * (uint32)drawer_count));
	// Located at PML2 + (8 * 512 * directory_count)
	// a.k.a. PT (page table, 4KB per entry)
	// Holds 512 entries * table_count, each entry maps 4KB, table = 2MB
	pm_t *pml1 = (pm_t*)(((uint32)pml2) + (sizeof(pm_t) * 512 * (uint32)directory_count));
	
	// Clear memory region where the page tables will reside
	mem_clear((uint8 *)pml4, sizeof(pm_t) * 512);
	mem_clear((uint8 *)pml3, sizeof(pm_t) * 512 * drawer_count);
	mem_clear((uint8 *)pml2, sizeof(pm_t) * 512 * directory_count);
	mem_clear((uint8 *)pml1, sizeof(pm_t) * 512 * table_count);

	// Set up pages, tables, directories and drawers in the cabinet :)
	for (p = 0; p < page_count; p ++){
		ptr = (uint64)(p * PAGE_SIZE);
		pml1[p].raw = ptr & PAGE_MASK;
		pml1[p].s.present = 1;
		pml1[p].s.writable = 1;
		pml1[p].s.write_through = 1;
		//pml1[p].s.cache_disable = 1;
		//pml1[p].s.global = 1;
	}
	for (t = 0; t < table_count; t ++){
		ptr = (uint64)(((uint32)pml1) + (sizeof(pm_t) * 512 * t));
		pml2[t].raw = ptr & PAGE_MASK;
		pml2[t].s.present = 1;
		pml2[t].s.writable = 1;
		pml2[t].s.write_through = 1;
		//pml2[t].s.cache_disable = 1;
	}
	for (d = 0; d < directory_count; d ++){
		ptr = (uint64)(((uint32)pml2) + (sizeof(pm_t) * 512 * d));
		pml3[d].raw = ptr & PAGE_MASK;
		pml3[d].s.present = 1;
		pml3[d].s.writable = 1;
		pml3[d].s.write_through = 1;
		//pml3[d].s.cache_disable = 1;
	}
	for (dr = 0; dr < drawer_count; dr ++){
		ptr = (uint64)(((uint32)pml3) + (sizeof(pm_t) * 512 * dr));
		pml4[dr].raw = ptr & PAGE_MASK;
		pml4[dr].s.present = 1;
		pml4[dr].s.writable = 1;
		pml4[dr].s.write_through = 1;
		//pml4[dr].s.cache_disable = 1;
	}

	// Set PML4 pointer address
	pml4_ptr32 = (uint32)pml4; // Point to our cabinet :)
}

/**
* Initialize Protected Mode
*/
void main32(){
	// Page map some memory (identity map)
	setup_pages(INIT_MEM);
}
