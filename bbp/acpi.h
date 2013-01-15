/**
* @file acpi.h
* ACPI functions
* @see acpi.c
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
} RSDP_t;
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
} SDTHeader_t;
/**
* Root System Descriptor table
*/
typedef struct {
	SDTHeader_t h;					// Standard ACPI header
	uint32 ptr;						// This actually is an array, but we're going to use it as an offset
} RSDT_t;
/**
* Generic Address structure
*/
typedef struct {
	uint8 address_space;
	uint8 bit_width;
	uint8 bit_offset;
	uint8 access_size;
	uint64 address;
} GAS_t;
/**
* Fixed ACPI Description Table structure
*/
typedef struct {
	SDTHeader_t h;					// Standard ACPI header
	uint32 firmware_ctrl;
	uint32 dsdt;
 
	// field used in ACPI 1.0; no longer in use, for compatibility only
	uint8 reserved;
 
	uint8  pref_pmp;
	uint16 sci_interrupt;
	uint32 smi_command_port;
	uint8  acpi_enable;
	uint8  acpi_disable;
	uint8  s4_bios_req;
	uint8  pstate_control;
	uint32 PM1aEventBlock;
	uint32 PM1bEventBlock;
	uint32 PM1aControlBlock;
	uint32 PM1bControlBlock;
	uint32 PM2ControlBlock;
	uint32 PMTimerBlock;
	uint32 GPE0Block;
	uint32 GPE1Block;
	uint8  PM1EventLength;
	uint8  PM1ControlLength;
	uint8  PM2ControlLength;
	uint8  PMTimerLength;
	uint8  GPE0Length;
	uint8  GPE1Length;
	uint8  GPE1Base;
	uint8  cstate_control;
	uint16 worst_C2_latency;
	uint16 worst_C3_latency;
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
	uint64 X_FirmwareControl;
	uint64 X_Dsdt;
 
	GAS_t X_PM1aEventBlock;
	GAS_t X_PM1bEventBlock;
	GAS_t X_PM1aControlBlock;
	GAS_t X_PM1bControlBlock;
	GAS_t X_PM2ControlBlock;
	GAS_t X_PMTimerBlock;
	GAS_t X_GPE0Block;
	GAS_t X_GPE1Block;
} FADT_t;
/**
* Multiple APIC Description Table structure
*/
typedef struct {
	SDTHeader_t h;
	uint32 lapic_add;			// Physical address of local APIC
	uint32 flags;				// Flags
	uint32 ptr;					// Local, IO and other APIC structures (we use it as an offset)
} MADT_t;
/**
* ACPI APIC structure header
*/
typedef struct {
	uint8 type;
	uint8 length;
} APICHeader_t;
/**
* Local APIC structure
*/
typedef struct {
	APICHeader_t h;
	uint8 processor_id;
	uint8 apic_id;
	uint32 flags;
} LocalAPIC_t;
/**
* I/O APIC strcuture
*/
typedef struct {
	APICHeader_t h;
	uint8 apic_id;
	uint8 reserved;
	uint32 apic_addr;
	uint32 gsi_base;
} IOAPIC_t;
/**
* Non Maskable Interrupt (NMI) structure
*/
typedef struct {
	APICHeader_t h;
	uint16 flags;
	uint32 gsi;
} NMI_t;
/**
* Local APIC structure
*/
typedef struct {
	APICHeader_t h;
	uint8 processor_id;
	uint16 flags;
	uint8 lint;
} LocalNMI_t;

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
