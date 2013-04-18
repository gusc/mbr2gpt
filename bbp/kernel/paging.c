/*

Memory paging functions
=======================

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

#include "paging.h"
#include "../config.h"

/**
* Page table structures
*/
static pm_t *pml4 = (pm_t *)PT_LOC; // a.k.a. PML4
//static pm_t *pml3; // a.k.a. PDPT (page directory pointer table)
//static pm_t *pml2; // a.k.a. PD (page directory)
//static pm_t *pml1; // a.k.a. PT (page table)

uint64 page_normalize_vaddr(uint64 vaddr){
	vaddr_t va;
	va.raw = vaddr;
	if ((va.s.pml4_idx & 0x100) != 0){
		va.s.canonical = 0xFFFF;
	} else {
		va.s.canonical = 0x0000;
	}
	return va.raw;
}

uint64 page_get_pml4_idx(uint64 vaddr){
	vaddr_t va;
	va.raw = page_normalize_vaddr(vaddr);
	return va.s.pml4_idx;
}
uint64 page_get_pml3_idx(uint64 vaddr){
	vaddr_t va;
	va.raw = page_normalize_vaddr(vaddr);
	return va.s.directory_idx;
}
uint64 page_get_pml2_idx(uint64 vaddr){
	vaddr_t va;
	va.raw = page_normalize_vaddr(vaddr);
	return va.s.table_idx;
}
uint64 page_get_pml1_idx(uint64 vaddr){
	vaddr_t va;
	va.raw = page_normalize_vaddr(vaddr);
	return va.s.page_idx;
}

pm_t page_get_pml4_entry(uint64 vaddr){
	vaddr_t va;
	pm_t pe;
	va.raw = page_normalize_vaddr(vaddr);
	pe.raw = pml4[va.s.pml4_idx].raw;
	return pe;
}
pm_t page_get_pml3_entry(uint64 vaddr){
	vaddr_t va;
	uint64 table_addr;
	pm_t *table;
	pm_t pe;
	va.raw = page_normalize_vaddr(vaddr);
	table_addr = pml4[va.s.pml4_idx].raw;
	table = (pm_t *)(table_addr & PAGE_MASK);
	pe.raw = table[va.s.directory_idx].raw;
	return pe;
}
pm_t page_get_pml2_entry(uint64 vaddr){
	vaddr_t va;
	uint64 table_addr;
	pm_t *table;
	pm_t pe;
	va.raw = page_normalize_vaddr(vaddr);
	table_addr = pml4[va.s.pml4_idx].raw;
	table = (pm_t *)(table_addr & PAGE_MASK);
	table_addr = table[va.s.directory_idx].raw;
	table = (pm_t *)(table_addr & PAGE_MASK);
	pe.raw = table[va.s.table_idx].raw;
	return pe;
}
pm_t page_get_pml1_entry(uint64 vaddr){
	vaddr_t va;
	uint64 table_addr;
	pm_t *table;
	pm_t pe;
	va.raw = page_normalize_vaddr(vaddr);
	table_addr = pml4[va.s.pml4_idx].raw;
	table = (pm_t *)(table_addr & PAGE_MASK);
	table_addr = table[va.s.directory_idx].raw;
	table = (pm_t *)(table_addr & PAGE_MASK);
	table_addr = table[va.s.table_idx].raw;
	table = (pm_t *)(table_addr & PAGE_MASK);
	pe.raw = table[va.s.page_idx].raw;
	return pe;
}

void page_set_pml4_entry(uint64 vaddr, pm_t pe){
	vaddr_t va;
	va.raw = page_normalize_vaddr(vaddr);
	pml4[va.s.pml4_idx].raw = pe.raw;
}
void page_set_pml3_entry(uint64 vaddr, pm_t pe){
	vaddr_t va;
	uint64 table_addr;
	pm_t *table;
	va.raw = page_normalize_vaddr(vaddr);
	table_addr = pml4[va.s.pml4_idx].raw;
	table = (pm_t *)(table_addr & PAGE_MASK);
	table[va.s.directory_idx].raw = pe.raw;
}
void page_set_pml2_entry(uint64 vaddr, pm_t pe){
	vaddr_t va;
	uint64 table_addr;
	pm_t *table;
	va.raw = page_normalize_vaddr(vaddr);
	table_addr = pml4[va.s.pml4_idx].raw;
	table = (pm_t *)(table_addr & PAGE_MASK);
	table_addr = table[va.s.directory_idx].raw;
	table = (pm_t *)(table_addr & PAGE_MASK);
	table[va.s.table_idx].raw = pe.raw;
}
void page_set_pml1_entry(uint64 vaddr, pm_t pe){
	vaddr_t va;
	uint64 table_addr;
	pm_t *table;
	va.raw = page_normalize_vaddr(vaddr);
	table_addr = pml4[va.s.pml4_idx].raw;
	table = (pm_t *)(table_addr & PAGE_MASK);
	table_addr = table[va.s.directory_idx].raw;
	table = (pm_t *)(table_addr & PAGE_MASK);
	table_addr = table[va.s.table_idx].raw;
	table = (pm_t *)(table_addr & PAGE_MASK);
	table[va.s.page_idx].raw = pe.raw;
}
