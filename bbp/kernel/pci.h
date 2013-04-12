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

#ifndef __pci_h
#define __pci_h

#include "common.h"

#define PCI_CONFIG_ADDRESS	0x0CF8
#define PCI_CONFIG_DATA		0x0CFC

#define PCI_REG_DEVID_VNDID	0x0
#define PCI_REG_STATUS_CMD	0x4
#define PCI_REG_CLS_PRG_REV	0x8
#define PCI_REG_BIST_TYPE	0xC

typedef union {
	struct {
		uint32 empty	: 2; // Always 0
		uint32 reg		: 6; // Register number
		uint32 fn		: 3; // Function number
		uint32 device	: 5; // Device number
		uint32 bus		: 8; // Bus number
		uint32 reserved	: 7; // Reserved
		uint32 enabled	: 1; // Enabled bit
	} s;
	uint32 raw;
} pci_addr_t;

/**
* Enumerate PCI bus
*/
void pci_init();
/**
* Read from PCI bus/device
* @param addr [in] - PCI address (bus, device, function, register)
* @return register value
*/
uint32 pci_read(pci_addr_t *addr);
/**
* Write to PCI bus/device
* @param addr [in] - PCI address (bus, device, function, register)
* @param data - data to write
*/
void pci_write(pci_addr_t *addr, uint32 data);


#endif /* __pci_h */