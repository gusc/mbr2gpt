/*

Kernel entry point
==================

This is where the fun part begins

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
#include "kmain.h"
#include "lib.h"

#if DEBUG == 1
	#include "debug_print.h"
#endif
#include "paging.h"
#include "apic.h"
#include "acpi.h"
#include "io.h"
#include "interrupts.h"
#include "pci.h"

/**
* Page table structures
*/
static pm_t *pml4; // a.k.a. PML4T
static pm_t *pml3; // a.k.a. PTD (page directory table)
static pm_t *pml2; // a.k.a. PD (page directory)
static pm_t *pml1; // a.k.a. PT (page table)

#if DEBUG == 1
/**
* List available ACPI tables on screen
*/
static void acpi_list();
#endif

/**
* Kernel entry point
*/
void kmain(){
#if DEBUG == 1
	// Clear the screen
	debug_clear(DC_WB);
	// Show something on the screen
	debug_print(DC_WB, "Long mode");
#endif

	// Initialize interrupts
	interrupt_init();

#if DEBUG == 1
	// Show memory ammount
	e820map_t *mem_map = (e820map_t *)E820_LOC;
	uint16 i;
	uint64 max_mem = 0;
	debug_print(DC_WB, "Memory map:");
	for (i = 0; i < mem_map->size; i ++){
		if (mem_map->entries[i].base + mem_map->entries[i].length > max_mem){
			max_mem = mem_map->entries[i].base + mem_map->entries[i].length;
		}
		debug_print(DC_WDG, "%x - %x = %d", mem_map->entries[i].base, mem_map->entries[i].length, mem_map->entries[i].type);
	}
	max_mem = max_mem / 1024 / 1024;
	debug_print(DC_WB, "RAM: %d", max_mem);
#endif
		
	// Initialize ACPI
	if (acpi_init()){
#if DEBUG == 1
		// List available ACPI tables
		acpi_list();
#endif
		// Initialize APIC
		//apic_init();
		// Map pages to APCI base location
		//pml4 = (pm_t*)PT_LOC; // @0x00100000
		//apic_base_t apic = apic_get_base();
		//video_print_int(sx, sy++, 0x07, apic.raw, 16);
		//video_print_int(sx, sy++, 0x07, apic_read_ioapic(apic, APIC_LAPIC_ID), 16);
		//video_print_int(sx, sy++, 0x07, apic_read_ioapic(apic, APIC_LAPIC_VERSION), 16);
		// Initialize PCI
		//pci_init();
	}

	// Test interrupt exceptions
	// division by zero:
	//uint32 a = 1;
	//uint32 b = 0;
	//uint32 c = a / b;
	// page fault:
	//char *xyz = (char *)0xFFFFFFFF;
	//*xyz = 'A';
	
	// Infinite loop
	while(true){}
}

#if DEBUG == 1
static void acpi_list(RSDP_t *rsdp){
	if (rsdp != null){
		SDTHeader_t *th;
		uint32 i;
		uint32 count;
		char sign[5] = "";
		mem_fill((uint8 *)sign, 5, 0);
		if (rsdp->revision == 0){
			debug_print(DC_WB, "ACPI v1.0");
			// ACPI version 1.0
			RSDT_t *rsdt = (RSDT_t *)((uint64)rsdp->RSDT_address);
			uint64 ptr;
			// Get count of other table pointers
			count = (rsdt->h.length - sizeof(SDTHeader_t)) / 4;
			for (i = 0; i < count; i ++){
				// Get an address of table pointer array
				ptr = (uint64)&rsdt->ptr;
				// Move on to entry i (32bits = 4 bytes) in table pointer array
				ptr += (i * 4);
				// Get the pointer of table in table pointer array
				th = (SDTHeader_t *)((uint64)(*((uint32 *)ptr)));
				mem_copy((uint8 *)sign, 4, (uint8 *)th->signature);
				debug_print(DC_WB, sign);
			}
		} else {
			debug_print(DC_WB, "ACPI v2.0+");
			// ACPI version 2.0+
			XSDT_t *xsdt = (XSDT_t *)rsdp->XSDT_address;
			uint64 ptr;
			// Get count of other table pointers
			count = (xsdt->h.length - sizeof(SDTHeader_t)) / 4;
			for (i = 0; i < count; i ++){
				// Get an address of table pointer array
				ptr = (uint64)&xsdt->ptr;
				// Move on to entry i (64bits = 8 bytes) in table pointer array
				ptr += (i * 8);
				// Get the pointer of table in table pointer array
				th = (SDTHeader_t *)(*((uint64 *)ptr));
				mem_copy((uint8 *)sign, 4, (uint8 *)th->signature);
				debug_print(DC_WB, sign);
			}
		}
	}
}
#endif
