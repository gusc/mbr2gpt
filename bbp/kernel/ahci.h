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

#ifndef __ahci_h
#define __ahci_h

#include "common.h"
#include "../config.h"

#define AHCI_DEV_SATA	0x00000101	// SATA drive
#define AHCI_DEV_SATAPI	0xEB140101	// SATAPI drive
#define AHCI_DEV_SEMB	0xC33C0101	// Enclosure management bridge
#define AHCI_DEV_PM		0x96690101	// Port multiplier


typedef volatile struct {
	uint32 clb;			// 0x00, command list base address, 1K-byte aligned
	uint32 clbu;		// 0x04, command list base address upper 32 bits
	uint32 fb;			// 0x08, FIS base address, 256-byte aligned
	uint32 fbu;			// 0x0C, FIS base address upper 32 bits
	uint32 is;			// 0x10, interrupt status
	uint32 ie;			// 0x14, interrupt enable
	uint32 cmd;			// 0x18, command and status
	uint32 rsv0;		// 0x1C, Reserved
	uint32 tfd;			// 0x20, task file data
	uint32 sig;			// 0x24, signature
	uint32 ssts;		// 0x28, SATA status (SCR0:SStatus)
	uint32 sctl;		// 0x2C, SATA control (SCR2:SControl)
	uint32 serr;		// 0x30, SATA error (SCR1:SError)
	uint32 sact;		// 0x34, SATA active (SCR3:SActive)
	uint32 ci;			// 0x38, command issue
	uint32 sntf;		// 0x3C, SATA notification (SCR4:SNotification)
	uint32 fbs;			// 0x40, FIS-based switch control
	uint32 rsv1[11];	// 0x44 ~ 0x6F, Reserved
	uint32 vendor[4];	// 0x70 ~ 0x7F, vendor specific
} ahci_hba_port_t;

typedef volatile struct {
	// 0x00 - 0x2B, Generic Host Control
	uint32 cap;			// 0x00, Host capability
	uint32 ghc;			// 0x04, Global host control
	uint32 is;			// 0x08, Interrupt status
	uint32 pi;			// 0x0C, Port implemented
	uint32 vs;			// 0x10, Version
	uint32 ccc_ctl;		// 0x14, Command completion coalescing control
	uint32 ccc_pts;		// 0x18, Command completion coalescing ports
	uint32 em_loc;		// 0x1C, Enclosure management location
	uint32 em_ctl;		// 0x20, Enclosure management control
	uint32 cap2;		// 0x24, Host capabilities extended
	uint32 bohc;		// 0x28, BIOS/OS handoff control and status
 	// 0x2C - 0x9F, Reserved
	uint8 rsv[0xA0-0x2C];
 	// 0xA0 - 0xFF, Vendor specific registers
	uint8 vendor[0x100-0xA0];
 	// 0x100 - 0x10FF, Port control registers
	ahci_hba_port_t ports[1];	// 1 ~ 32
} ahci_hba_t;


/**
* Initialize AHCI driver
* @return false if no AHCI controller has been found
*/
bool ahci_init();

#endif
