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
* Scan through memory and locate RSDP.
* @return RSDP_t * - pointer to RSDP structure in memory
*/
RSDP_t *acpi_find();
/**
* Locate ACPI table - you must run acpi_find() first
* @param const char *[in] - table signature (name)
* @return SDTHeader_t * - pointer to table header (from here on you can locate all the other data)
*/
SDTHeader_t *acpi_table(const RSDP_t *pointer, const char signature[4]);

#endif
