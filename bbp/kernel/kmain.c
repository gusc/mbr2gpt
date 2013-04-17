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
#include "io.h"
#include "interrupts.h"
#include "paging.h"
#include "acpi.h"
#include "apic.h"
#include "pci.h"
#if DEBUG == 1
	#include "debug_print.h"
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
		debug_print(DC_WDG, "%x - %x = %d", mem_map->entries[i].base, mem_map->entries[i].base + mem_map->entries[i].length, mem_map->entries[i].type);
	}
	max_mem = max_mem / 1024 / 1024;
	debug_print(DC_WB, "RAM: %d", max_mem);
#endif
	
	// Initialize ACPI
	if (acpi_init()){
#if DEBUG == 1
		// List available ACPI tables
		//acpi_list();
#endif
		// Initialize APIC
		apic_init();

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
