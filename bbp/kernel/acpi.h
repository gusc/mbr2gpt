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

#ifndef __acpi_h
#define __acpi_h

#include "common.h"

/**
* Root System Descriptor Pointer
* See ACPI specs
*/
typedef struct {
	// Version 1.0
	char signature[8];
	uint8 checksum;
	char oem_id[6];
	uint8 revision;
	uint32 RSDT_address;
	// Version 2.0
	uint32 length;
	uint64 XSDT_address;
	uint8 extended_checksum;
	uint8 reserved[3];
} __PACKED RSDP_t;
/**
* Standard System Descriptor Table header structure
* This is common to all ACPI tables
*/
typedef struct {
	char signature[4];
	uint32 length;
	uint8 revision;
	uint8 checksum;
	char oem_id[6];
	char oem_table_id[8];
	uint32 oem_revision;
	uint32 creator_id;
	uint32 creator_revision;
} __PACKED SDTHeader_t;
/**
* Root System Descriptor table
*/
typedef struct {
	SDTHeader_t h;					// Standard ACPI header
	uint32 ptr;						// This actually is an array, we're using only first entry
} __PACKED RSDT_t;
/**
* eXtended System Descriptor Table
*/
typedef struct {
	SDTHeader_t h;					// Standard ACPI header
	uint64 ptr;						// This actually is an array, we're using only first entry
} __PACKED XSDT_t;
/**
* Secondary System Descriptor table
*/
typedef struct {
	SDTHeader_t h;					// Standard ACPI header
	uint8 ptr;						// AML bytecode
} __PACKED SSDT_t;
/**
* Differentiated System Descriptor table
*/
typedef struct {
	SDTHeader_t h;					// Standard ACPI header
	uint8 ptr;						// AML bytecode
} __PACKED DSDT_t;
/**
* Firmware ACPI Control Structure
*/
typedef struct {
	char signature[4];
	uint32 length;
	uint32 hw_signature;
	uint32 fw_vector;
	uint32 global_lock;
	uint32 flags;

	uint64 x_fw_vector;
	uint8 version;
	uint8 reserved[3];
	uint32 ospm_flags;
	uint8 reserved2[24];
} __PACKED FACS_t;
/**
* Generic Address structure
*/
typedef struct {
	uint8 address_space;
	uint8 bit_width;
	uint8 bit_offset;
	uint8 access_size;
	uint64 address;
} __PACKED GAS_t;
/**
* Fixed ACPI Description Table structure
*/
typedef struct {
	SDTHeader_t h;					// Standard ACPI header
	FACS_t *firmware_ctrl;			// Pointer to FACS table
	DSDT_t *dsdt;					// Pointer to DSDT table
 
	// field used in ACPI 1.0; no longer in use, for compatibility only
	uint8 reserved;
 
	uint8  pref_pmp;
	uint16 sci_interrupt;
	uint32 smi_command_port;
	uint8  acpi_enable;
	uint8  acpi_disable;
	uint8  s4_bios_req;
	uint8  pstate_control;
	uint32 pm1a_event_block;
	uint32 pm1b_event_block;
	uint32 pm1a_control_block;
	uint32 pm1b_control_block;
	uint32 pm2_control_block;
	uint32 pm_timer_block;
	uint32 gpe0_block;
	uint32 gpe1_block;
	uint8  pm1_event_length;
	uint8  pm1_control_length;
	uint8  pm2_control_length;
	uint8  pm_timer_length;
	uint8  gpe0_length;
	uint8  gpe1_length;
	uint8  gpe1_base;
	uint8  cstate_control;
	uint16 worst_c2_latency;
	uint16 worst_c3_latency;
	uint16 flush_size;
	uint16 flush_stride;
	uint8  duty_offset;
	uint8  duty_width;
	uint8  day_alarm;
	uint8  month_alarm;
	uint8  century;
 
	// reserved in ACPI 1.0; used since ACPI 2.0+
	uint16 boot_arch_flags;
 
	uint8  reserved2;
	uint32 flags;
 
	// 12 byte structure; see below for details
	GAS_t reset_reg;
 
	uint8 reset_value;
	uint8 reserved3[3];
 
	// 64bit pointers - Available on ACPI 2.0+
	uint64 x_firmware_control;
	uint64 x_dsdt;
 
	GAS_t x_pm1a_event_block;
	GAS_t x_pm1b_event_block;
	GAS_t x_pm1a_control_block;
	GAS_t x_pm1b_control_block;
	GAS_t x_pm2_control_block;
	GAS_t x_pm_timer_block;
	GAS_t x_gpe0_block;
	GAS_t x_gpe1_block;
} __PACKED FADT_t;
/**
* Multiple APIC Description Table structure
*/
typedef struct {
	SDTHeader_t h;
	uint32 lapic_add;			// Physical address of local APIC
	uint32 flags;				// Flags
	uint32 ptr;					// Local, IO and other APIC structures (we use it as an offset)
} __PACKED MADT_t;
/**
* ACPI APIC structure header
*/
typedef struct {
	uint8 type;
	uint8 length;
} __PACKED APICHeader_t;
/**
* Local APIC structure
*/
typedef struct {
	APICHeader_t h;
	uint8 processor_id;
	uint8 apic_id;
	uint32 flags;
} __PACKED LocalAPIC_t;
/**
* I/O APIC strcuture
*/
typedef struct {
	APICHeader_t h;
	uint8 apic_id;
	uint8 reserved;
	uint32 apic_addr;
	uint32 gsi_base;
} __PACKED IOAPIC_t;
/**
* Non Maskable Interrupt (NMI) structure
*/
typedef struct {
	APICHeader_t h;
	uint16 flags;
	uint32 gsi;
} __PACKED NMI_t;
/**
* Local APIC structure
*/
typedef struct {
	APICHeader_t h;
	uint8 processor_id;
	uint16 flags;
	uint8 lint;
} __PACKED LocalNMI_t;

/**
* Scan through memory and locate RSDP.
* @return pointer to RSDP structure in memory
*/
RSDP_t *acpi_find();
/**
* Locate ACPI table - you must run acpi_find() first
* @param [in] signature - table signature (table name)
* @return pointer to table header (from here on you can locate all the other data)
*/
SDTHeader_t *acpi_table(const char signature[4]);

#endif
