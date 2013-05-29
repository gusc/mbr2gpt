/*

Helper functions for PCI operations
===================================


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
#include "lib.h"
#include "io.h"
#include "pci.h"
#if DEBUG == 1
	#include "debug_print.h"
#endif

/*
static uint32 pci_get_addr(uint16 bus, uint8 device, uint8 function, uint8 reg){
	uint32 addr = 0x80000000;
	addr |= ((uint32)bus) << 16;
	addr |= (((uint32)device) & 0x1F) << 11;
	addr |= (((uint32)function) & 0x7) << 8;
	addr |= (uint32)(reg & 0xfc);
	return addr;
}
*/



static pci_dev_t _cache[256];
static uint8 _cache_len = 0;

/**
* Get secondary bus number from PCI-to-PCI bridge
*/
static uint16 pci_get_secondary_bus(uint16 bus, uint8 device, uint8 function){
	pci_addr_t addr;
	uint32 secondary_bus = 0;
	if (bus < 256 && device < 32 && function < 8){
		addr.raw = 0;
		addr.s.enabled = 1;
		addr.s.bus = bus;
		addr.s.device = device;
		addr.s.function = function;
		addr.s.reg = 6;
		secondary_bus = pci_read(addr.raw);
		return (uint16)((secondary_bus >> 8) & 0xFF);
	}
	return bus;
}

void pci_init(){
	pci_header_t header;
	uint16 bus = 0;

	// Recursive scan - thanks OSDev Wiki
	if (pci_get_header(0, 0, 0, &header)){
		if ((header.type & 0x80) != 0){
			// Multiple PCI host controllers
			for (bus = 0; bus < 8; bus ++){
				if (pci_get_header(0, 0, bus, &header)){
					if (header.vendor_id != 0xFFFF){
						// Valid PCI host controller
						pci_enum_bus(bus);
					}
				}
			}
		} else {
			// Single PCI host controller
			pci_enum_bus(0);
		}
	}
}

bool pci_find_device(pci_dev_t *dev, uint8 class_id, uint8 subclass_id){
	uint16 i = 0;
	for (i = 0; i < _cache_len; i ++){
		if (_cache[i].class_id == class_id && _cache[i].subclass_id == subclass_id){
			mem_copy((uint8 *)dev, sizeof(pci_dev_t), (uint8 *)&_cache[i]);
			return true;
		}
	}
	return false;
}

bool pci_get_header(uint16 bus, uint8 device, uint8 function, pci_header_t *h){
	pci_addr_t addr;
	uint32 rows[4] = {0, 0, 0, 0};
	uint8 row;
	if (bus < 256 && device < 32 && function < 8){
		addr.raw = 0;
		addr.s.enabled = 1;
		addr.s.bus = bus;
		addr.s.device = device;
		addr.s.function = function;
		for (row = 0; row < 4; row ++){
			addr.s.reg = row;
			rows[row] = pci_read(addr.raw);
		}
		mem_copy((uint8 *)h, 16, (uint8 *)rows);
		return true;
	}
	return false;
}

void pci_enum_bus(uint16 bus){
	uint8 device = 0;
	for (; device < 32; device ++){
		pci_enum_device(bus, device);
	}
}

void pci_enum_device(uint16 bus, uint8 device){
	pci_header_t header;
	uint8 function = 0;
	if (pci_get_header(bus, device, 0, &header)){
		if (header.vendor_id != 0xFFFF){
			if ((header.type & 0x80) != 0){ 
				// Multifunctional device
				for (function = 1; function < 8; function ++){
					pci_enum_function(bus, device, function);
				}
			} else {
				// Single function device
				pci_enum_function(bus, device, 0);
			}
		}
	}
}

void pci_enum_function(uint16 bus, uint8 device, uint8 function){
	pci_header_t header;
	uint16 secondary_bus = 0;
	if (pci_get_header(bus, device, function, &header)){
		if (header.vendor_id != 0xFFFF){
			_cache[_cache_len].address.raw = 0;
			_cache[_cache_len].address.s.bus = bus;
			_cache[_cache_len].address.s.device = device;
			_cache[_cache_len].address.s.function = function;
			_cache[_cache_len].class_id = header.class_id;
			_cache[_cache_len].subclass_id = header.subclass_id;
			_cache[_cache_len].vendor_id = header.vendor_id;
			_cache[_cache_len].device_id = header.device_id;
			_cache_len ++;
#if DEBUG == 1
			debug_print(DC_BW, "pci%u:%u:%u, class:%x:%x:%x, vendor:%x:%x", (uint64)bus, (uint64)device, (uint64)function, (uint64)header.class_id, (uint64)header.subclass_id, (uint64)header.prog_if, (uint64)header.vendor_id, (uint64)header.device_id);
#endif
			if (header.class_id == 0x06 && header.subclass_id == 0x04){
				secondary_bus = pci_get_secondary_bus(bus, device, function);
				if (secondary_bus != bus){
					pci_enum_bus(secondary_bus);
				}
			}
		}
	}
}

uint32 pci_read(uint32 addr){
	uint32 data;
	outd(PCI_CONFIG_ADDRESS, addr);
	data = ind(PCI_CONFIG_DATA);
	return data;
}

void pci_write(uint32 addr, uint32 data){
	outd(PCI_CONFIG_ADDRESS, addr);
	outd(PCI_CONFIG_DATA, data);
}

/*uint32 pci_read(uint32 addr){
	uint32 data;
	asm volatile ("outl %%eax, %%dx" : : "d"(PCI_CONFIG_ADDRESS), "a"(addr));
	asm volatile("inl %%dx, %%eax" : "=a"(data) : "d"(PCI_CONFIG_DATA));
	return data;
}

void pci_write(uint32 addr, uint32 data){
	asm volatile ("outl %%eax, %%dx" : : "d"(PCI_CONFIG_ADDRESS), "a"(addr));
	asm volatile ("outl %%eax, %%dx" : : "d"(PCI_CONFIG_DATA), "a"(data));
}*/
