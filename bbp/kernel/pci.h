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
#include "../config.h"

#define PCI_CONFIG_ADDRESS	0x0CF8
#define PCI_CONFIG_DATA		0x0CFC

#define PCI_REG_DEVID_VNDID	0x0
#define PCI_REG_STATUS_CMD	0x4
#define PCI_REG_CLS_PRG_REV	0x8
#define PCI_REG_BIST_TYPE	0xC

/**
* PCI address structure
*/
typedef union {
	struct {
		uint32 empty	: 2; // Always 0
		uint32 reg		: 6; // Register number
		uint32 function	: 3; // Function number
		uint32 device	: 5; // Device number
		uint32 bus		: 8; // Bus number
		uint32 reserved	: 7; // Reserved
		uint32 enabled	: 1; // Enabled bit
	} s;
	uint32 raw;
} pci_addr_t;
/**
* PCI configuration space header structure
*/
typedef struct {
	uint16 vendor_id;
	uint16 device_id;
	uint16 command;
	uint16 status;
	uint8 revision_id;
	uint8 prog_if;
	uint8 subclass_id;
	uint8 class_id;
	uint8 cache_line_size;
	uint8 latency_timer;
	uint8 type;
	uint8 bist;
} pci_header_t;
/**
* Standard PCI device configuration space structure
*/
typedef struct {
	pci_header_t header;
	uint32 bar[6];
	uint32 cis_ptr;
	uint16 sub_vendor_id;
	uint16 subsystem_id;
	uint32 exp_rom;
	uint8 compat;
	uint8 reserved[7];
	uint8 int_line;
	uint8 int_pin;
	uint8 min_grant;
	uint8 max_latency;
} pci_device_t;

/**
* Enumerate PCI bus
*/
void pci_init();
/**
* Locate PCI device by class and subclass
* @param class_id - class code
* @param subclass_id - sub-class
* @return PCI address (check if it's not 0!)
*/
pci_addr_t pci_find_device(uint8 class_id, uint8 subclass_id);
/**
* Read PCI device header
* @param addr - PCI address
* @return a pointer to header structure
*/
pci_header_t *pci_get_header(pci_addr_t addr);
/**
* Read PCI device configuration structure
* @param addr - PCI address
* @return a pointer to configuration structure
*/
pci_device_t *pci_get_config(pci_addr_t addr);
/**
* Read from PCI bus/device
* @param addr - PCI address
* @return register value
*/
uint32 pci_read(pci_addr_t addr);
/**
* Write to PCI bus/device
* @param addr - PCI address
* @param data - data to write
*/
void pci_write(pci_addr_t addr, uint32 data);


#if DEBUG == 1
/**
* List available PCI devices on screen
*/
void pci_list();
#endif


#endif /* __pci_h */