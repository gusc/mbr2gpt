/*

APIC, xAPIC, x2APIC functions
=============================

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
#include "apic.h"
#include "msr.h"
#include "acpi.h"
#if DEBUG == 1
	#include "debug_print.h"
#endif

//#define MAX_LAPIC

//LocalAPIC_t *lapic[

bool apic_init(){
	char apic[4] = {'A', 'P', 'I', 'C'};
	uint32 length;
	MADT_t *madt = (MADT_t *)acpi_table(apic);
	APICHeader_t *ah;
	if (madt != null){
		length = (madt->h.length - sizeof(MADT_t) + 4);
#if DEBUG == 1
			debug_print(DC_WGR, "APIC size: %d", length);
#endif
		ah = (APICHeader_t *)(&madt->ptr);
		/*while (length > 0){
#if DEBUG == 1
			debug_print(DC_WGR, "APIC type: %d", ah->type);
#endif

			length -= ah->length;
			ah = (APICHeader_t *)(((uint64)ah) + ah->length);
		}*/
	}
}

apic_base_t apic_get_base(){
	apic_base_t addr;
	uint32 low, high;
	msr_read(MSR_IA32_APIC_BASE, &low, &high);
	addr.raw = (low & 0xFFFFF000) | ((uint64)(high & 0x0F) << 32);
	return addr;
}

void apic_set_base(apic_base_t addr){
	uint32 low = (addr.raw & 0xFFFFF000) | 0x800;
	uint32 high = (addr.raw >> 32) & 0x0F;
	msr_write(MSR_IA32_APIC_BASE, low, high);
}

uint32 apic_read_ioapic(apic_base_t addr, uint32 reg){
	uint32 volatile *ioapic = (uint32 volatile *)(addr.raw);
	ioapic[0] = (reg & 0xFFFF);
	return ioapic[4];
}

void apic_write_ioapic(apic_base_t addr, uint32 reg, uint32 data){
	uint32 volatile *ioapic = (uint32 volatile *)(addr.raw);
	ioapic[0] = (reg & 0xFFFF);
	ioapic[4] = data;
}
