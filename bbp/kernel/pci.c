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
#include "io.h"
#include "pci.h"
#if DEBUG == 1
	#include "debug_print.h"
#endif

void pci_init(){
	uint16 bus;
	uint8 device;
	uint32 data;
	uint16 vendor_id;
	uint8 class_id;
	uint8 subclass_id;
	pci_addr_t addr;
	addr.raw = 0;
	addr.s.enabled = 1;
	// Bruteforce for now
	for (bus = 0; bus < 256; bus ++){
		for (device = 0; device < 32; device ++){
			addr.s.bus = bus;
			addr.s.device = device;
			addr.s.reg = 0;
			data = pci_read(&addr);
			vendor_id = (uint16)data;
			if (vendor_id != 0xFFFF){
				addr.s.reg = 8;
				data = pci_read(&addr);
				class_id = (uint8)(data >> 24);
				subclass_id = (uint8)(data >> 16);
#if DEBUG == 1
				debug_print(DC_BW, "Bus: %x, Dev: %x, Class: %x, Sub: %x", bus, device, class_id, subclass_id);
#endif
			}
		}
	}
}

bool pci_get_header(uint16 bus, uint8 device, pci_header_t *h){
	uint32 data;
	pci_addr_t addr;
	if (bus < 256 && device <= 32){
		addr.raw = 0;
		addr.s.enabled = 1;
		addr.s.bus = bus;
		addr.s.device = device;
		addr.s.reg = 0;
		data = pci_read(&addr);
		h->vendor_id = (uint16)data;
		h->device_id = (uint16)(data >> 16);
		if (h->vendor_id != 0xFFFF){

			return true;
		}
	}
	return false;
}

uint32 pci_read(pci_addr_t *addr){
	uint32 data;
	outd(PCI_CONFIG_ADDRESS, addr->raw);
	data = ind(PCI_CONFIG_DATA);
	return data;
}

void pci_write(pci_addr_t *addr, uint32 data){
	outd(PCI_CONFIG_ADDRESS, addr->raw);
	outd(PCI_CONFIG_DATA, data);
}
