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
#include "paging.h"
#if DEBUG == 1
	#include "debug_print.h"
#endif

static LocalAPIC_t *_lapic[256];
static uint64 _lapic_count = 0;
static uint64 _lapic_addr;

static IOAPIC_t *_ioapic[256];
static uint64 _ioapic_count = 0;

static void lapic_init(){
	apic_base_t apic = apic_get_base();
	// Address is 4KB aligned
	_lapic_addr = (apic.raw & PAGE_MASK);
#if DEBUG == 1
	debug_print(DC_WB, "Local APIC @%x", _lapic_addr);
	if (apic.s.bsp){
		debug_print(DC_WB, "Boot CPU");
	}
#endif
	// Map pages to Local APIC and disable cache
	pm_t pe;
	uint64 i;
	page_map_mmio(_lapic_addr);
	for (i = 0; i < 4; i ++){
		pe = page_get_pml_entry(_lapic_addr, i);
		pe.s.cache_disable = 1;
		page_set_pml_entry(_lapic_addr, i, pe);
	}

	// Initialize Local APIC
	uint32 val = apic_read_reg(APIC_LAPIC_VERSION);
#if DEBUG == 1
	debug_print(DC_WBL, "Version: %d", val);
#endif

	// Initialize Other Local APICs if this is a bootstrap processor
	if (apic.s.bsp){
		for (i = 0; i < _lapic_count; i ++){
#if DEBUG == 1
		debug_print(DC_WBL, "CPU_ID:APIC_ID = %d:%d", _lapic[i]->processor_id, _lapic[i]->apic_id);
#endif		
			//TODO: signal other CPUs to start
		}
	}
}

static void ioapic_init(){
	// Address is 4KB aligned
	uint64 i;
	for (i = 0; i < _ioapic_count; i ++){
		uint64 ioapic_addr = (_ioapic[i]->apic_addr & PAGE_MASK);
#if DEBUG == 1
		debug_print(DC_WB, "IO APIC @%x", ioapic_addr);
		debug_print(DC_WB, "IOAPIC ID:%d", _ioapic[i]->apic_id);
#endif
		// Map pages to Local APIC and disable cache
		pm_t pe;
		uint64 l;
		page_map_mmio(ioapic_addr);
		for (l = 0; l < 4; l ++){
			pe = page_get_pml_entry(ioapic_addr, i);
			pe.s.cache_disable = 1;
			page_set_pml_entry(ioapic_addr, i, pe);
		}

		// TODO: setup IRQs
	}
}

bool apic_init(){
	char apic[4] = {'A', 'P', 'I', 'C'};
	MADT_t *madt = (MADT_t *)acpi_table(apic);
	if (madt != null){
		// Gather Local and IO APIC(s)
		_lapic_addr = (uint64)madt->lapic_addr;
		
		// Enumerate APICs
		uint64 length = (madt->h.length - sizeof(MADT_t) + 4);
		APICHeader_t *ah = (APICHeader_t *)(&madt->ptr);
		while (length > 0){
#if DEBUG == 1
			//debug_print(DC_WGR, "APIC type: %d", ah->type);
#endif
			switch (ah->type){
				case APIC_TYPE_LAPIC:
					// Test if it's enabled - if not - don't touch it
					if ((((LocalAPIC_t *)ah)->flags & 1) != 0){
						_lapic[_lapic_count] = (LocalAPIC_t *)ah;
						_lapic_count ++;
					}
					break;
				case APIC_TYPE_IOAPIC:
					_ioapic[_ioapic_count] = (IOAPIC_t *)ah;
					_ioapic_count ++;
					break;
			}
			length -= ah->length;
			ah = (APICHeader_t *)(((uint64)ah) + ah->length);
		}
#if DEBUG == 1
		debug_print(DC_WB, "CPU count:%d", _lapic_count);
#endif

		// Initialize Local APIC
		lapic_init();
		// Initialize IO APIC
		ioapic_init();
	}
}

apic_base_t apic_get_base(){
	apic_base_t addr;
	msr_read(MSR_IA32_APIC_BASE, &addr.raw);
	return addr;
}
void apic_set_base(apic_base_t addr){
	msr_write(MSR_IA32_APIC_BASE, addr.raw);
}

uint32 apic_read_reg(uint64 reg){
	uint32 volatile *apic = (uint32 volatile *)(_lapic_addr + reg);
	return *apic;
}
void apic_write_reg(uint64 reg, uint32 value){
	uint32 volatile *apic = (uint32 volatile *)(_lapic_addr + reg);
	(*apic) = value;
}

uint32 apic_read_ioapic(uint64 addr, uint32 reg){
	uint32 volatile *ioapic = (uint32 volatile *)(addr);
	ioapic[0] = (reg & 0xFFFF);
	return ioapic[4];
}
void apic_write_ioapic(uint64 addr, uint32 reg, uint32 data){
	uint32 volatile *ioapic = (uint32 volatile *)(addr);
	ioapic[0] = (reg & 0xFFFF);
	ioapic[4] = data;
}
