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

#include "../config.h"
#include "paging.h"
#include "lib.h"
#if DEBUG == 1
	#include "debug_print.h"
#endif

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

/**
* Page table structures
*/
static pm_t *_pml4 = (pm_t *)PT_LOC;
static uint64 _page_offset = PT_LOC;

static uint64 _total_mem = 0;
static uint64 _available_mem = 0;

static uint64 *_page_frames;
static uint64 _page_count = 0;

// Get bitset index
#define BIT_INDEX(b) (b / 64)
// Get bit offset in bitset
#define BIT_OFFSET(b) (b % 64)

/**
* Mark frame allocated
* @param paddr - physical address to mark
*/
static void page_set_frame(uint64 paddr){
    uint64 page = paddr / 4069;
    uint64 idx = BIT_INDEX(page);
    uint64 offset = BIT_OFFSET(page);
	_page_frames[idx] |= (0x1 << offset);
}
/**
* Mark frame free
* @param paddr - physical address to mark
*/
static void page_clear_frame(uint64 paddr){
    uint64 page = paddr / 4069;
    uint64 idx = BIT_INDEX(page);
    uint64 offset = BIT_OFFSET(page);
	_page_frames[idx] &= ~(0x1 << offset);
}
/**
* Check if frame is allocated or free
* @param paddr - physical address to check
* @return true if set false if not
*/
static bool page_check_frame(uint64 paddr){
    uint64 page = paddr / 4069;
    uint64 idx = BIT_INDEX(page);
    uint64 offset = BIT_OFFSET(page);
    return (_page_frames[idx] & (0x1 << offset));
}

/**
* Sort memory map in ascending order
*/
static void sort_e820(e820map_t *mem_map){
	uint64 i = 0;
	// Do the bubble sort to make them in ascending order
	e820entry_t e;
	uint64 swapped  = 1;
	uint64 count = mem_map->size;
	while (count > 0 && swapped ){
		i = 0;
		swapped  = 0;
		while (i < count - 1){
			if (mem_map->entries[i].base > mem_map->entries[i + 1].base){
				e = mem_map->entries[i];
				mem_map->entries[i] = mem_map->entries[i + 1];
				mem_map->entries[i + 1] = e;
				swapped  = 1;
			}
			i ++;
		}
		count --;
	}
	// Get total RAM
	for (i = 0; i < mem_map->size; i ++){
#if DEBUG == 1
		//debug_print(DC_WB, "%x -> %x (%d)", mem_map->entries[i].base, mem_map->entries[i].base + mem_map->entries[i].length, mem_map->entries[i].type);
#endif
		if (mem_map->entries[i].type != kMemReserved){
			if (mem_map->entries[i].base + mem_map->entries[i].length > _total_mem){
				_total_mem = mem_map->entries[i].base + mem_map->entries[i].length;
			}
			if (mem_map->entries[i].type == kMemOk){
				_available_mem += mem_map->entries[i].length;
			}
		}
	}
}

void page_init(){
	// Read E820 memory map and mark used regions
	e820map_t *mem_map = (e820map_t *)E820_LOC;
	// Sort memory map
	sort_e820(mem_map);
		
	// Single page (PML1 entry) holds 4KB of RAM
	uint64 page_count = INIT_MEM / PAGE_SIZE;
	if (INIT_MEM % PAGE_SIZE > 0){
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

	// Determine the end of PMLx structures to add new ones
	_page_offset += (sizeof(pm_t) * 512) * (1 + drawer_count + directory_count + table_count);
	// Calculate total frame count
	_page_count = _total_mem / PAGE_SIZE;
	// Allocate frame bitset at the next page boundary
	_page_frames = (uint64 *)_page_offset;
	// Clear bitset
	mem_fill((uint8 *)_page_count, _page_count / 8, 0);
	// Move offset further
	_page_offset += (_page_count / 8);
	// Align the offset
	if ((_page_offset & PAGE_MASK) != _page_offset){
		_page_offset &= PAGE_MASK;
		_page_offset += PAGE_SIZE;
	}

#if DEBUG == 1
	debug_print(DC_WB, "Frames: %d", _page_count);
#endif

	// Determine unsuable memory regions
	uint64 i;
	uint64 paddr_from;
	uint64 paddr_to;
	for (i = 0; i < mem_map->size; i ++){
		if (mem_map->entries[i].type != kMemOk){
			// Mark all unusable regions used
			paddr_from = (mem_map->entries[i].base & PAGE_MASK);
			paddr_to = ((mem_map->entries[i].base + mem_map->entries[i].length) & PAGE_MASK);
			while (paddr_from < paddr_to){
				page_set_frame(paddr_from);
				paddr_from += PAGE_SIZE;
			}
		}
	}
}
uint64 page_total_mem(){
	return _total_mem;
}
uint64 page_available_mem(){
	return _available_mem;
}
uint64 page_normalize_vaddr(uint64 vaddr){
	vaddr_t va;
	va.raw = vaddr;
	if ((va.s.drawer_idx & 0x100) != 0){
		va.s.canonical = 0xFFFF;
	} else {
		va.s.canonical = 0x0000;
	}
	return va.raw;
}
uint64 page_map(uint64 paddr){
	// Do the identity map
	vaddr_t va;
	pm_t *pml3;
	pm_t *pml2;
	pm_t *pml1;
	va.raw = page_normalize_vaddr(paddr);
	if (!_pml4[va.s.drawer_idx].s.present){
		page_set_frame(_page_offset);
		pml3 = (pm_t *)_page_offset;
		mem_fill((uint8 *)pml3, sizeof(pm_t) * 512, 0);
		_pml4[va.s.drawer_idx].raw = (uint64)pml3;
		_pml4[va.s.drawer_idx].s.present = 1;
		_pml4[va.s.drawer_idx].s.writable = 1;
		_pml4[va.s.drawer_idx].s.write_through = 1;
		//_pml4[va.s.drawer_idx].s.cache_disable = 1;
		_page_offset += (sizeof(pm_t) * 512);
	}
	pml3 = (pm_t *)(_pml4[va.s.drawer_idx].raw & PAGE_MASK);
	if (!pml3[va.s.directory_idx].s.present){
		page_set_frame(_page_offset);
		pml2 = (pm_t *)_page_offset;
		mem_fill((uint8 *)pml2, sizeof(pm_t) * 512, 0);
		pml3[va.s.directory_idx].raw = (uint64)pml2;
		pml3[va.s.directory_idx].s.present = 1;
		pml3[va.s.directory_idx].s.writable = 1;
		pml3[va.s.directory_idx].s.write_through = 1;
		//pml3[va.s.directory_idx].s.cache_disable = 1;
		_page_offset += (sizeof(pm_t) * 512);
	}
	pml2 = (pm_t *)(pml3[va.s.directory_idx].raw & PAGE_MASK);
	if (!pml2[va.s.table_idx].s.present){
		page_set_frame(_page_offset);
		pml1 = (pm_t *)_page_offset;
		mem_fill((uint8 *)pml1, sizeof(pm_t) * 512, 0);
		pml2[va.s.table_idx].raw = (uint64)pml1;
		pml2[va.s.table_idx].s.present = 1;
		pml2[va.s.table_idx].s.writable = 1;
		pml2[va.s.table_idx].s.write_through = 1;
		//pml2[va.s.table_idx].s.cache_disable = 1;
		_page_offset += (sizeof(pm_t) * 512);
	}
	pml1 = (pm_t *)(pml2[va.s.table_idx].raw & PAGE_MASK);
	if (!pml1[va.s.page_idx].s.present){
		pml1[va.s.page_idx].raw = (paddr & PAGE_MASK);
		pml1[va.s.page_idx].s.present = 1;
		pml1[va.s.page_idx].s.writable = 1;
		pml1[va.s.page_idx].s.write_through = 1;
		//pml1[va.s.page_idx].s.cache_disable = 1;
	}	
	return va.raw;
}
uint64 page_resolve(uint64 vaddr){
	vaddr_t va;
	uint64 paddr = 0;
	pm_t *table;
	va.raw = vaddr;
	if (_pml4[va.s.drawer_idx].s.present){
		table = (pm_t *)(_pml4[va.s.drawer_idx].raw & PAGE_MASK);
		if (table[va.s.directory_idx].s.present){
			table = (pm_t *)(table[va.s.directory_idx].raw & PAGE_MASK);
			if (table[va.s.table_idx].s.present){
				table = (pm_t *)(table[va.s.table_idx].raw & PAGE_MASK);
				if (table[va.s.page_idx].s.present){
					paddr = va.s.offset; // set offset
					paddr |= (table[va.s.page_idx].raw & PAGE_MASK); // merge page aligned address
				}
			}
		}
	}
	return paddr;
}

pm_t page_get_pml_entry(uint64 vaddr, uint8 level){
	vaddr_t va;
	va.raw = vaddr;
	pm_t *table = _pml4;
	if (level >= 3){
		return table[va.s.drawer_idx];
	}
	table = (pm_t *)(table[va.s.drawer_idx].raw & PAGE_MASK);
	if (level == 2){
		return table[va.s.directory_idx];
	}
	table = (pm_t *)(table[va.s.directory_idx].raw & PAGE_MASK);
	if (level == 1){
		return table[va.s.table_idx];
	}
	table = (pm_t *)(table[va.s.table_idx].raw & PAGE_MASK);
	return table[va.s.page_idx];
}

void page_set_pml_entry(uint64 vaddr, uint8 level, pm_t pe){
	vaddr_t va;
	va.raw = page_normalize_vaddr(vaddr);
	pm_t *table = _pml4;
	if (level >= 3){
		table[va.s.drawer_idx].raw = pe.raw;
	}
	table = (pm_t *)(table[va.s.drawer_idx].raw & PAGE_MASK);
	if (level == 2){
		table[va.s.directory_idx].raw = pe.raw;
	}
	table = (pm_t *)(table[va.s.directory_idx].raw & PAGE_MASK);
	if (level == 1){
		table[va.s.table_idx].raw = pe.raw;
	}
	table = (pm_t *)(table[va.s.table_idx].raw & PAGE_MASK);
	table[va.s.page_idx].raw = pe.raw;
}
