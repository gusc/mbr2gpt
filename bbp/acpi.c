#include "acpi.h"
#include "memory.h"

static const char sign[9] = "RSD PTR ";
static RSDP_t *rsdp = null;

static uint8 acpi_checksum(char *block, uint32 len){
	uint32 sum = 0;
	while (len--){
		sum += *(block++);
	}
	return (uint8)(sum);
}

RSDP_t *acpi_find(){
	if (rsdp == null){
		rsdp = (RSDP_t *)0x87c00; // We start at the end of our BBP code
		do {
			if (mem_cmp((uint8 *)rsdp->signature, (uint8 *)sign, 8)){
				if (rsdp->revision == 0){
					if (acpi_checksum((char *)rsdp, sizeof(char) * 20) == 0){ // Revision 1.0 checksum
						return rsdp;
					}
				} else {
					if (acpi_checksum((char *)rsdp, sizeof(RSDP_t)) == 0){ // Revision 2.0+ checksum
						return rsdp;
					}
				}
			}
			rsdp = (RSDP_t *)((uint32)rsdp + 0x10);
		} while ((uint32)rsdp < 0x100000);
		rsdp = null;
	}
	return rsdp;
}

SDTHeader_t *acpi_table(const char signature[4]){
	if (rsdp != null){
		SDTHeader_t *h;
		SDTHeader_t *th;
		if (rsdp->revision == 0){
			h = (SDTHeader_t *)rsdp->RSDT_address;
		} else {
			h = (SDTHeader_t *)rsdp->XSDT_address;
		}
		for (uint32 i = 0; i < (h->length - sizeof(SDTHeader_t)) / 4; i ++){
			th = (SDTHeader_t *)(h + sizeof(SDTHeader_t) + (i * 4));
			if (mem_cmp((uint8 *)th->signature, (uint8 *)signature, 4)){
				return th;
			}
		}
	}
	return null;
}
