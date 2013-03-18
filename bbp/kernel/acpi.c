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

#include "acpi.h"
#include "memory.h"

RSDP_t *rsdp = null;

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

RSDP_t *acpi_find(){
	const char sign[9] = "RSD PTR ";
	if (rsdp == null){
		rsdp = (RSDP_t *)0x80000; // We start at the beginning of EBDA
		do {
			if (mem_cmp((uint8 *)rsdp->signature, (uint8 *)sign, 8)){
				if (rsdp->revision == 0){
					if (acpi_checksum((uint8 *)rsdp, sizeof(uint8) * 20) == 0){ // Revision 1.0 checksum
						return rsdp;
					}
				} else {
					if (acpi_checksum((uint8 *)rsdp, sizeof(RSDP_t)) == 0){ // Revision 2.0+ checksum
						return rsdp;
					}
				}
			}
			rsdp = (RSDP_t *)((uint64)rsdp + 0x10);
		} while ((uint64)rsdp < 0x100000); // Up until 1MB mark
		rsdp = null;
	}
	return rsdp;
}

SDTHeader_t *acpi_table(const char signature[4]){
	if (rsdp != null){
		SDTHeader_t *th;
		uint32 i;
		uint32 count;
		if (rsdp->revision == 0){
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
				if (mem_cmp((uint8 *)th->signature, (uint8 *)signature, 4)){
					if (acpi_checksum((uint8 *)th, th->length) == 0){
						return th;
					}
				}
			}
		} else {
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
				if (mem_cmp((uint8 *)th->signature, (uint8 *)signature, 4)){
					if (acpi_checksum((uint8 *)th, th->length) == 0){
						return th;
					}
				}
			}
		}
	}
	return null;
}
