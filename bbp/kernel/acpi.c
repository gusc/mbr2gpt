/*

ACPI functions
==============

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
#include "acpi.h"
#include "lib.h"
#include "paging.h"
#include "io.h"
#if DEBUG == 1
	#include "debug_print.h"
#endif

RSDP_t *_rsdp = null;

/**
* Calculate ACPI checksum
* @param [in] block - memory address of a first byte of ACPI structure
* @param len - structure size in memory
* @return checksum value
*/
static uint8 acpi_checksum(uint8 *block, uint64 len){
	uint32 sum = 0;
	while (len--){
		sum += *(block++);
	}
	return (uint8)(sum);
}
/**
* Find root system descriptor pointer (RSDP)
* @retun true if found
*/
static bool acpi_find(){
	const char sign[9] = "RSD PTR ";
	if (_rsdp == null){
		_rsdp = (RSDP_t *)0x80000; // We start at the beginning of EBDA
		do {
			if (mem_compare((uint8 *)_rsdp->signature, (uint8 *)sign, 8)){
				if (_rsdp->revision == 0){
					if (acpi_checksum((uint8 *)_rsdp, sizeof(uint8) * 20) == 0){ // Revision 1.0 checksum
						// Map RSDT address
						page_map_mmio((uint64)_rsdp->RSDT_address);
						return true;
					}
				} else {
					if (acpi_checksum((uint8 *)_rsdp, sizeof(RSDP_t)) == 0){ // Revision 2.0+ checksum
						// Map XSDT address
						page_map_mmio(_rsdp->XSDT_address);
						return true;
					}
				}
			}
			_rsdp = (RSDP_t *)((uint64)_rsdp + 0x10);
		} while ((uint64)_rsdp < 0x100000); // Up until 1MB mark
		_rsdp = null;
	}
	return false;
}
/**
* Map pages to ACPI tables
*/
static void acpi_map(){
	if (_rsdp != null){
		SDTHeader_t *th;
		uint64 i;
		uint64 j;
		uint64 count;
		uint64 ptr;
		if (_rsdp->revision == 0){
			// ACPI version 1.0
			RSDT_t *rsdt = (RSDT_t *)((uint64)_rsdp->RSDT_address);
			// Get count of other table pointers
			count = (rsdt->h.length - sizeof(SDTHeader_t)) / 4;
			for (i = 0; i < count; i ++){
				// Get an address of table pointer array
				ptr = (uint64)&rsdt->ptr;
				// Move on to entry i (32bits = 4 bytes) in table pointer array
				ptr += (i * 4);
				// Map the page
				page_map_mmio(ptr);
				// Get the pointer of table in table pointer array
				th = (SDTHeader_t *)((uint64)(*((uint32 *)ptr)));
				// If the length is greater than a page, map additional pages 
				if (th->length > PAGE_SIZE){
					for (j = (ptr & PAGE_MASK) + PAGE_SIZE; j < ((ptr + th->length) & PAGE_MASK); j += PAGE_SIZE){
						page_map_mmio(j);
					}
				}
			}
		} else {
			// ACPI version 2.0+
			XSDT_t *xsdt = (XSDT_t *)_rsdp->XSDT_address;
			// Get count of other table pointers
			count = (xsdt->h.length - sizeof(SDTHeader_t)) / 8;
			for (i = 0; i < count; i ++){
				// Get an address of table pointer array
				ptr = (uint64)&xsdt->ptr;
				// Move on to entry i (64bits = 8 bytes) in table pointer array
				ptr += (i * 8);
				// Map the page
				page_map_mmio(ptr);
				// Get the pointer of table in table pointer array
				th = (SDTHeader_t *)(*((uint64 *)ptr));
				// If the length is greater than a page, map additional pages 
				if (th->length > PAGE_SIZE){
					for (j = (ptr & PAGE_MASK) + PAGE_SIZE; j < ((ptr + th->length) & PAGE_MASK); j += PAGE_SIZE){
						page_map_mmio(j);
					}
				}
			}
		}
	}
}

bool acpi_init(){
	if (acpi_find()){
		// Map pages to ACPI tables, so we don't get page faults
		acpi_map();

		char facp[4] = {'F', 'A', 'C', 'P'};
		FADT_t *fadt = (FADT_t *)acpi_table(facp);
		if (fadt != null){
			// Enable ACPI
			if ((fadt->pm1a_control_block & 0x1) == 0){ // Only if SCI_EN is not set
				if (fadt->smi_command_port > 0){ // and SMI_CMD is set
					if (fadt->acpi_enable > 0){ // and ACPI_ENABLE is set
						outb((uint16)fadt->smi_command_port, fadt->acpi_enable);
					}
				}
			}

			//FACS_t *facs = (FACS_t *)((uint64)fadt->firmware_ctrl);
			
			//DSDT_t *dsdt = (DSDT_t *)((uint64)fadt->dsdt);
			// TODO: parse DSDT

			//char ssdt_sig[4] = {'S', 'S', 'D', 'T'};
			//SSDT_t *ssdt = (SSDT_t *)acpi_table(ssdt_sig);
			//if (ssdt != null){
				// TODO: parse SSDT
			//}

			return true;
		}
	}
	return false;
}

RSDP_t *acpi_rsdp(){
	return _rsdp;
}

SDTHeader_t *acpi_table(const char signature[4]){
	if (_rsdp != null){
		SDTHeader_t *th;
		uint32 i;
		uint32 count;
		uint64 ptr;
		if (_rsdp->revision == 0){
			// ACPI version 1.0
			RSDT_t *rsdt = (RSDT_t *)((uint64)_rsdp->RSDT_address);
			// Get count of other table pointers
			count = (rsdt->h.length - sizeof(SDTHeader_t)) / 4;
			for (i = 0; i < count; i ++){
				// Get an address of table pointer array
				ptr = (uint64)&rsdt->ptr;
				// Move on to entry i (32bits = 4 bytes) in table pointer array
				ptr += (i * 4);
				// Get the pointer of table in table pointer array
				th = (SDTHeader_t *)((uint64)(*((uint32 *)ptr)));
				if (mem_compare((uint8 *)th->signature, (uint8 *)signature, 4)){
					if (acpi_checksum((uint8 *)th, th->length) == 0){
						return th;
					}
				}
			}
		} else {
			// ACPI version 2.0+
			XSDT_t *xsdt = (XSDT_t *)_rsdp->XSDT_address;
			// Get count of other table pointers
			count = (xsdt->h.length - sizeof(SDTHeader_t)) / 8;
			for (i = 0; i < count; i ++){
				// Get an address of table pointer array
				ptr = (uint64)&xsdt->ptr;
				// Move on to entry i (64bits = 8 bytes) in table pointer array
				ptr += (i * 8);
				// Get the pointer of table in table pointer array
				th = (SDTHeader_t *)(*((uint64 *)ptr));
				if (mem_compare((uint8 *)th->signature, (uint8 *)signature, 4)){
					if (acpi_checksum((uint8 *)th, th->length) == 0){
						return th;
					}
				}
			}
		}
	}
	return null;
}

#if DEBUG == 1
void acpi_list(){
	if (_rsdp != null){
		SDTHeader_t *th;
		uint32 i;
		uint32 count;
		char sign[5] = "";
		
		debug_print(DC_WB, "RSDP @%x", (uint64)_rsdp);

		if (_rsdp->revision == 0){
			// ACPI version 1.0
			debug_print(DC_WB, "ACPI v1.0");
			debug_print(DC_WB, "XSDT @%x", (uint64)_rsdp->RSDT_address);
			
			RSDT_t *rsdt = (RSDT_t *)((uint64)_rsdp->RSDT_address);
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
				mem_fill((uint8 *)sign, 5, 0);
				mem_copy((uint8 *)sign, 4, (uint8 *)th->signature);
				debug_print(DC_WB, "%s @%x", sign, (uint64)th);
			}
		} else {
			// ACPI version 2.0+
			debug_print(DC_WB, "ACPI v%d", (uint32)_rsdp->revision);
			debug_print(DC_WB, "XSDT @%x", _rsdp->XSDT_address);
			
			XSDT_t *xsdt = (XSDT_t *)_rsdp->XSDT_address;
			uint64 ptr;
			// Get count of other table pointers
			count = (xsdt->h.length - sizeof(SDTHeader_t)) / 8;
			for (i = 0; i < count; i ++){
				// Get an address of table pointer array
				ptr = (uint64)&xsdt->ptr;
				// Move on to entry i (64bits = 8 bytes) in table pointer array
				ptr += (i * 8);
				// Get the pointer of table in table pointer array
				th = (SDTHeader_t *)(*((uint64 *)ptr));
				mem_fill((uint8 *)sign, 5, 0);
				mem_copy((uint8 *)sign, 4, (uint8 *)th->signature);
				debug_print(DC_WB, "%s @%x", sign, (uint64)th);
			}
		}
	}
}
#endif