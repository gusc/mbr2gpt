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

// Device signatures
#define AHCI_DEV_SATA	0x00000101	// SATA drive
#define AHCI_DEV_SATAPI	0xEB140101	// SATAPI drive
#define AHCI_DEV_SEMB	0xC33C0101	// Enclosure management bridge
#define AHCI_DEV_PM		0x96690101	// Port multiplier

// AHCI Specification 1.3 data structures

/**
* HBA Port structure
*/
typedef volatile struct {
	uint64 clb;					// command list base address, 1K-uint8 aligned
	uint64 fb;					// FIS base address, 256-uint8 aligned
	// Interrupt status
	struct {
		uint32 dhrs			:1;	// Device to Host Register FIS Interrupt
		uint32 pss			:1;	// PIO Setup FIS Interrupt
		uint32 dss			:1;	// DMA Setup FIS Interrupt
		uint32 sdbs			:1;	// Set Device Bits Interrupt
		uint32 ufs			:1;	// Unknown FIS Interrupt
		uint32 dps			:1;	// Descriptor Processed
		uint32 pcs			:1;	// Port Connect Change Status
		uint32 dmps			:1;	// Device Mechanical Presence Status
		uint32 reserved1	:14;
		uint32 prcs			:1;	// PhyRdy Change Status
		uint32 ipms			:1;	// Incorrect Port Multiplier Status
		uint32 ofs			:1;	// Overflow Status
		uint32 reserved2	:1;
		uint32 infs			:1;	// Interface Non-fatal Error Status
		uint32 ifs			:1;	// Interface Fatal Error Status
		uint32 hbds			:1;	// Host Bus Data Error Status
		uint32 hbfs			:1;	// Host Bus Fatal Error Status
		uint32 tfes			:1;	// Task File Error Status
		uint32 cpds			:1;	// Cold Port Detect Status
	} is;
	// Interrupt enable
	struct {
		uint32 dhre			:1;	// Device to Host Register FIS Interrupt Enable
		uint32 pse			:1;	// PIO Setup FIS Interrupt Enable
		uint32 dse			:1;	// DMA Setup FIS Interrupt Enable
		uint32 sdbe			:1;	// Set Device Bits FIS Interrupt Enable
		uint32 ufe			:1;	// Unknown FIS Interrupt Enable
		uint32 dpe			:1;	// Descriptor Processed Interrupt Enable
		uint32 pce			:1;	// Port Change Interrupt Enable
		uint32 dmpe			:1;	// Device Mechanical Presence Enable
		uint32 reserved1	:14;
		uint32 prce			:1;	// PhyRdy Change Interrupt Enable
		uint32 ipme			:1;	// Incorrect Port Multiplier Enable
		uint32 ofe			:1;	// Overflow Enable
		uint32 reserved2	:1;
		uint32 infe			:1;	// Interface Non-fatal Error Enable
		uint32 ife			:1;	// Interface Fatal Error Enable
		uint32 hbde			:1;	// Host Bus Data Error Enable
		uint32 hbfe			:1;	// Host Bus Fatal Error Enable
		uint32 tfee			:1;	// Task File Error Enable
		uint32 cpde			:1;	// Cold Presence Detect Enable
	} ie;
	// Command and status
	struct {
		uint32 st			:1;	// Start
		uint32 sud			:1;	// Spin-Up Device
		uint32 pod			:1;	// Power On Device
		uint32 clo			:1;	// Command List Override
		uint32 fre			:1;	// FIS Receive Enable
		uint32 reserved1	:3;
		uint32 ccs			:5;	// Current Command Slot
		uint32 mpss			:1;	// Mechanical Presence Switch State
		uint32 fr			:1;	// FIS Receive Running
		uint32 cr			:1;	// Command List Running
		uint32 cps			:1;	// Cold Presence State
		uint32 pma			:1;	// Port Multiplier Attached
		uint32 hpcp			:1;	// Hot Plug Capable Port
		uint32 mpsp			:1;	// Mechanical Presence Switch Attached to Port
		uint32 cpd			:1;	// Cold Presence Detection
		uint32 esp			:1;	// External SATA Port
		uint32 fbscp		:1;	// FIS-based Switching Capable Port
		uint32 apste		:1;	// Automatic Partial to Slumber Transitions Enabled
		uint32 atapi		:1;	// Device is ATAPI
		uint32 dlae			:1;	// Drive LED on ATAPI Enable
		uint32 dlpe			:1;	// Aggressive Link Power Management Enable
		uint32 asp			:1;	// Aggressive Slumber / Partial
		uint32 icc			:4;	// Interface Communication Control
	} cmd;					
	uint32 reserved1;
	// Task file data
	struct {
		uint8 status;
		uint8 error;
		uint16 reserved;
	} tfd;
	uint32 sig;					// Signature
	// SATA status (SCR0:SStatus)
	struct {
		uint32 det			:4;	// Device Detection
		uint32 spd			:4;	// Current Interface Speed
		uint32 ipm			:4;	// Interface Power Management
		uint32 reserved		:20;
	} ssts;
	// SATA control (SCR2:SControl)
	struct {
		uint32 det			:4;	// Device Detection Initialization
		uint32 spd			:4;	// Speed Allowed
		uint32 ipm			:4;	// Interface Power Management Transitions Allowed
		uint32 reserved		:20;
	} sctl;
	// SATA error (SCR1:SError)
	struct {
		// Error
		struct {
			uint16 i		:1;	// Recovered Data Integrity Error
			uint16 m		:1;	// Recovered Communications Error
			uint16 reserved1:6;
			uint16 t		:1;	// Transient Data Integrity Error
			uint16 c		:1;	// Persistent Communication or Data Integrity Error
			uint16 p		:1;	// Protocol Error
			uint16 e		:1;	// Internal Error
			uint16 reserved2:4;
		} err;
		// Diagnostics
		struct {
			uint16 n		:1;	// PhyRdy Change
			uint16 i		:1;	// Phy Internal Error
			uint16 w		:1;	// Comm Wake
			uint16 b		:1;	// 10B to 8B Decode Error
			uint16 d		:1;	// Disparity Error
			uint16 c		:1;	// CRC Error
			uint16 h		:1;	// Handshake Error
			uint16 s		:1;	// Link Sequence Error
			uint16 t		:1;	// Transport state transition error
			uint16 f		:1;	// Unknown FIS Type
			uint16 x		:1;	// Exchanged
			uint16 reserved	:5;
		} diag;
	} serr;
	uint32 sact;				// SATA active (SCR3:SActive)
	uint32 ci;					// Command issue
	uint32 sntf;				// SATA notification (SCR4:SNotification)
	// FIS-based switch control
	struct {
		uint32 en			:1;	// Enable
		uint32 dec			:1;	// Device Error Clear
		uint32 sde			:1;	// Single Device Error
		uint32 reserved1	:5;
		uint32 dev			:4;	// Device To Issue
		uint32 ado			:4;	// Active Device Optimization
		uint32 dwe			:4;	// Device With Error
		uint32 reserved2	:12;
	} fbs;
	uint32 reserved2[11];
	uint32 vendor[4];			// Vendor specific
} ahci_hba_port_t;
/**
* HBA structure
*/
typedef volatile struct {
	// Host capability
	struct {
		uint32 np			:4;	// Number of Ports
		uint32 sxs			:1;	// External SATA (eSATA) supported
		uint32 ems			:1;	// Enclosure Management supported
		uint32 cccs			:1; // Command Completion Coalescing supported
		uint32 ncs			:5;	// Number of Command Slots
		uint32 psc			:1;	// Partial State Capable
		uint32 ssc			:1;	// Slumber State Capable
		uint32 pmd			:1;	// PIO Multiple DRQ Block
		uint32 fbss			:1; // FIS-based Switching Supported
		uint32 spm			:1;	// Supports Port Multiplier
		uint32 sam			:1;	// Supports AHCI mode only
		uint32 reserved1	:1;
		uint32 iis			:4;	// Interface Speed Support
		uint32 sclo			:1;	// Supports Command List Override
		uint32 sal			:1;	// Supports Activity LED
		uint32 salp			:1;	// Supports Aggressive Link Power Management
		uint32 sss			:1; // Supports Staggered Spin-up
		uint32 smps			:1;	// Supports Mechanical Presence Switch
		uint32 ssntf		:1;	// Supports SNotification Register
		uint32 sncq			:1;	// Supports Native Command Queuing
		uint32 s64a			:1;	// Supports 64-bit Addressing
	} cap;
	// Global host control
	struct {
		uint32 hr			:1;	// HBA Reset
		uint32 ie			:1;	// Interrupt Enable
		uint32 mrsm			:1;	// MSI Revert to Single Message
		uint32 reserved		:28;
		uint32 ae			:1;	// AHCI Enable
	} ghc;
	uint32 is;					// Interrupt status
	uint32 pi;					// Port implemented
	uint32 vs;					// Version
	// Command completion coalescing control
	struct {
		uint32 en			:1; // Enable
		uint32 reserved		:2;
		uint32 interrupt	:5;	// Interrupt
		uint32 cc			:8; // Command Completions
		uint32 tv			:16;// Timeout Value
	} ccc_ctl;
	uint32 ccc_ports;			// Command completion coalescing ports
	// Enclosure management location
	struct {
		uint16 size;
		uint16 offset;
	} em_loc;
	// Enclosure management control
	struct {
		uint32 sts_mr		:1;	// Message Received
		uint32 reserved1	:7;
		uint32 ctl_tm		:1; // Transmit Message
		uint32 ctl_rst		:1;	// Reset
		uint32 reserved2	:6;
		uint32 supp_led		:1;	// LED Message Types
		uint32 supp_safte	:1;	// SAF-TE Enclosure Management Messages
		uint32 supp_ses2	:1;	// SES-2 Enclosure Management Messages
		uint32 supp_sgpio	:1;	// SGPIO Enclosure Management Messages
		uint32 reserved3	:4;
		uint32 attr_smb		:1;	// Single Message Buffer 
		uint32 attr_xmt		:1;	// Transmit Only
		uint32 attr_alhd	:1;	// Activity LED Hardware Driven
		uint32 attr_pm		:1;	// Port Multiplier Support
		uint32 reserved4	:4;
	} em_ctl;
	// Host capabilities extended
	struct {
		uint32 boh			:1;	// BIOS/OS Handoff
		uint32 nvmp			:1;	// NVMHCI Present
		uint32 apst			:1;	// Automatic Partial to Slumber Transitions
		uint32 reserved		:29;
	} cap2;
	// BIOS/OS handoff control and status
	struct {
		uint32 bos			:1;	// BIOS Owned Semaphore
		uint32 oos			:1;	// OS Owned Semaphore
		uint32 sooe			:1;	// SMI on OS Ownership Change Enable
		uint32 ooc			:1;	// OS Ownership Change
		uint32 bb			:1;	// BIOS Busy
		uint32 reserved		:27;
	} bohc;				
 	uint8 reserved[116];
 	uint8 vendor[96];			// Vendor specific registers
 	ahci_hba_port_t ports[1];	// Port control registers
} ahci_hba_t;

typedef volatile struct {
	uint8 fis_type;				// FIS_TYPE_REG_H2D
	uint8 pmport			:4;	// Port multiplier
	uint8 reserved1			:3;
	uint8 cmd				:1;	// 1: Command, 0: Control
	uint8 command;				// Command register
	uint8 featurel;				// Feature register, 7:0
	uint8 lba0;					// LBA low register, 7:0
	uint8 lba1;					// LBA mid register, 15:8
	uint8 lba2;					// LBA high register, 23:16
	uint8 device;				// Device register
	uint8 lba3;					// LBA register, 31:24
	uint8 lba4;					// LBA register, 39:32
	uint8 lba5;					// LBA register, 47:40
	uint8 featureh;				// Feature register, 15:8
	uint8 countl;				// Count register, 7:0
	uint8 counth;				// Count register, 15:8
	uint8 icc;					// Isochronous command completion
	uint8 control;				// Control register
	uint8 reserved2[4];
} ahci_fis_reg_h2d_t;

typedef volatile struct {
	uint8 fis_type;				// FIS_TYPE_REG_D2H
	uint8 pmport			:4;	// Port multiplier
	uint8 reserved1			:2;
	uint8 interrupt			:1;	// Interrupt bit
	uint8 reserved2			:1;
	uint8 status;				// Status register
	uint8 error;				// Error register
	uint8 lba0;					// LBA low register, 7:0
	uint8 lba1;					// LBA mid register, 15:8
	uint8 lba2;					// LBA high register, 23:16
	uint8 device;				// Device register
	uint8 lba3;					// LBA register, 31:24
	uint8 lba4;					// LBA register, 39:32
	uint8 lba5;					// LBA register, 47:40
	uint8 reserved3;
	uint8 countl;				// Count register, 7:0
	uint8 counth;				// Count register, 15:8
	uint8 reserved4[6];
} ahci_fis_reg_d2h_t;

typedef volatile struct {
	uint8 fis_type;				// FIS_TYPE_DATA
	uint8 pmport			:4; // Port multiplier
	uint8 reserved1			:4;	
	uint8 reserved2[2];
	uint32 data[1];				// Payload
} ahci_fis_data_t;

typedef volatile struct {
	uint8 fis_type;				// FIS_TYPE_PIO_SETUP
	uint8 pmport			:4;	// Port multiplier
	uint8 reserved1			:1;
	uint8 direction			:1;	// Data transfer direction, 1 - device to host
	uint8 interrupt			:1;	// Interrupt bit
	uint8 reserved2			:1;
	uint8 status;				// Status register
	uint8 error;				// Error register
	uint8 lba0;					// LBA low register, 7:0
	uint8 lba1;					// LBA mid register, 15:8
	uint8 lba2;					// LBA high register, 23:16
	uint8 device;				// Device register
	uint8 lba3;					// LBA register, 31:24
	uint8 lba4;					// LBA register, 39:32
	uint8 lba5;					// LBA register, 47:40
	uint8 reserved3;
	uint8 countl;				// Count register, 7:0
	uint8 counth;				// Count register, 15:8
	uint8 reserved4;		
	uint8 e_status;				// New value of status register
	uint16 tc;					// Transfer count
	uint8 reserved5[2];
} ahci_fis_pio_t;

typedef volatile struct {
	uint8 fis_type;				// FIS_TYPE_DMA_SETUP
 	uint8 pmport			:4;	// Port multiplier
	uint8 reserved1			:1;
	uint8 direction			:1;	// Data transfer direction, 1 - device to host
	uint8 interrupt			:1;	// Interrupt bit
	uint8 auto_act			:1;	// Auto-activate. Specifies if DMA Activate FIS is needed
	uint8 reserved2[2];
	uint64 dma_buff_id;			// DMA Buffer Identifier. Used to Identify DMA buffer in host memory. SATA Spec says host specific and not in Spec. Trying AHCI spec might work.
	uint32 reserved3;
	uint32 dma_buff_offset;		//uint8 offset into buffer. First 2 bits must be 0
	uint32 trans_count;			//Number of uint8s to transfer. Bit 0 must be 0
	uint32 reserved4;
} ahci_fis_dma_t;

/*
typedef volatile struct {
	ahci_fis_dma_t dsfis;		// DMA Setup FIS
	uint32 pad1;
	ahci_fis_pio_t psfis;		// PIO Setup FIS
	uint32 pad2[3];
	ahci_fis_reg_d2h_t rfis;	// Register – Device to Host FIS
	uint32 pad3;
	ahci_fis_dev_t sdbfis;		// Set Device Bit FIS
 	uint8 ufis[64];
 	uint32 reserved[96];
} ahci_fis_t;
*/

typedef volatile struct {
	// DW0 - Description Information
	struct {
		uint32 cfl			:5;	// Command FIS length in uint32S, 2 ~ 16
		uint32 a			:1;	// ATAPI
		uint32 w			:1;	// Write, 1: H2D, 0: D2H
		uint32 p			:1;	// Prefetchable
		uint32 r			:1;	// Reset
		uint32 b			:1;	// BIST
		uint32 c			:1;	// Clear busy upon R_OK
		uint32 reserved		:1;
		uint32 pmp			:4;	// Port multiplier port
		uint32 prdtl		:16;// Physical region descriptor table length in entries
	} desc;
	// DW1 - Command Status
	uint32 prdbc;				// Physical region descriptor uint8 count transferred
	// DW2 - Command Table Base Address
	uint64	ctba;
	// DW4 - 7 (reserved)
	uint32	reserved[4];
} ahci_hba_cmd_header_t;

// Check device type
static uint32 ahci_get_type(ahci_hba_port_t *port){
	if (port->ssts.det != 3){
		return 0;
	}
	if (port->ssts.ipm != 1){
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
			}
		}
		pi >>= 1;
		i ++;
	}
}

bool ahci_init(){
	uint64 abar = 0;
	ahci_hba_t *hba;
	pci_device_t dev;
#if DEBUG == 1
	debug_print(DC_WB, "SATA Controller count: %d", pci_num_device(0x1, 0x6));
#endif
	pci_addr_t addr = pci_get_device(0x1, 0x6, 0);
	if (addr.raw != 0){
		// Get AHCI controller configuration
		pci_get_config(&dev, addr);
		// Get ABAR (AHCI Base Address)
		abar = (uint64)dev.bar[5];
		// Map the page
		page_map_mmio(abar);
		hba = (ahci_hba_t *)abar;
#if DEBUG == 1
		debug_print(DC_WB, "SATA controller at %u:%u", addr.s.bus, addr.s.device);
		debug_print(DC_WB, "     BAR:0x%x", abar);
		debug_print(DC_WB, "     Num Ports:%d", hba->cap.np);
		debug_print(DC_WB, "     Num Commands:%d", hba->cap.ncs);
		debug_print(DC_WB, "     64-bit addresing:%d", hba->cap.s64a);
		debug_print(DC_WB, "     Version:%x", hba->vs);
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
