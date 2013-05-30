/*

AHCI driver
===========


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
#include "pci.h"
#include "paging.h"
#include "ahci.h"
#if DEBUG == 1
	#include "debug_print.h"
#endif

// Check device type
static uint32 ahci_get_type(ahci_hba_port_t *port){
	uint32 ssts = port->ssts;
	uint8 ipm = (ssts >> 8) & 0x0F;
	uint8 det = ssts & 0x0F;
 
	if (det != 3){
		return 0;
	}
	if (ipm != 1){
		return 0;
	}
	switch (port->sig){
		case AHCI_DEV_SATAPI:
		case AHCI_DEV_SEMB:
		case AHCI_DEV_PM:
			return port->sig;
		default:
			return AHCI_DEV_SATA;
	}
}
static void ahci_probe_port(ahci_hba_t *hba){
	uint32 pi = hba->pi;
	int i = 0;
	while (i < 32) {
		if (pi & 1) {
			int dt = ahci_get_type(&hba->ports[i]);
			if (dt == AHCI_DEV_SATA) {
				debug_print(DC_WGR, "SATA drive found at port %d\n", i);
			} else if (dt == AHCI_DEV_SATAPI) {
				debug_print(DC_WGR, "SATAPI drive found at port %d\n", i);
			} else if (dt == AHCI_DEV_SEMB) {
				debug_print(DC_WGR, "SEMB drive found at port %d\n", i);
			} else if (dt == AHCI_DEV_PM) {
				debug_print(DC_WGR, "PM drive found at port %d\n", i);
			} else {
				debug_print(DC_WGR, "No drive found at port %d\n", i);
			}
		}
		pi >>= 1;
		i ++;
	}
}

bool ahci_init(){
	uint64 abar = 0;
	ahci_hba_t *hba;
	pci_addr_t addr = pci_find_device(0x1, 0x6);
	if (addr.raw != 0){
		// Get AHCI controller configuration
		pci_device_t *dev = pci_get_config(addr);
		// Get ABAR (AHCI Base Address)
		abar = (uint64)dev->bar[5];
		// Map the page
		page_map(abar);
		hba = (ahci_hba_t *)abar;
#if DEBUG == 1
		debug_print(DC_WB, "SATA controller at %u:%u", addr.s.bus, addr.s.device);
		debug_print(DC_WB, "     BAR:0x%x", abar);
		debug_print(DC_WB, "     Ports:%b", hba->vs);
		debug_print(DC_WB, "     Capbility:%x", hba->cap);
		ahci_probe_port(hba);
#endif
		return true;
#if DEBUG == 1
	} else {
		debug_print(DC_WB, "SATA controller was not found");
#endif
	}
	return false;
}
