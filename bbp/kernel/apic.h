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

#ifndef __apic_h
#define __apic_h

#include "common.h"

//
// APIC register selector definitions
//

#define APIC_LAPIC_ID		0x20 // Local APIC ID Register (Read/Write)
#define APIC_LAPIC_VERSION	0x30 // Local APIC Version Register (Read Only)
#define APIC_TPR			0x80 // Task Priority Register (Read/Write)
#define APIC_APR			0x90 // Arbitration Priority Register (Read Only)
#define APIC_PPR			0xA0 // Processor Priority Register (Read Only)
#define APIC_EOIR			0xB0 // EOI Register (Write Only)
#define APIC_RRR			0xC0 // Remote Read Register (Read Only)
#define APIC_LDR			0xD0 // Logical Destination Register (Read/Write)
#define APIC_DFR			0xE0 // Destination Format Register (Read/Write)
#define APIC_SIVR			0xF0 // Spurious Interrupt Vector Register (Read/Write)
#define APIC_ISR1			0x0100 // In-Service Register bits 31:0 (Read Only)
#define APIC_ISR2			0x0110 // In-Service Register bits 63:32 (Read Only)
#define APIC_ISR3			0x0120 // In-Service Register bits 95:64 (Read Only)
#define APIC_ISR4			0x0130 // In-Service Register bits 127:96 (Read Only)
#define APIC_ISR5			0x0140 // In-Service Register bits 159:128 (Read Only)
#define APIC_ISR6			0x0150 // In-Service Register bits 191:160 (Read Only)
#define APIC_ISR7			0x0160 // In-Service Register bits 223:192 (Read Only)
#define APIC_ISR8			0x0170 // In-Service Register bits 255:224 (Read Only)
#define APIC_TMR1			0x0180 // Trigger Mode Register bits 31:0 (Read Only)
#define APIC_TMR2			0x0190 // Trigger Mode Register bits 63:32 (Read Only)
#define APIC_TMR3			0x01A0 // Trigger Mode Register bits 95:64 (Read Only)
#define APIC_TMR4			0x01B0 // Trigger Mode Register bits 127:96 (Read Only)
#define APIC_TMR5			0x01C0 // Trigger Mode Register bits 159:128 (Read Only)
#define APIC_TMR6			0x01D0 // Trigger Mode Register bits 191:160 (Read Only)
#define APIC_TMR7			0x01E0 // Trigger Mode Register bits 223:192 (Read Only)
#define APIC_TMR8			0x01F0 // Trigger Mode Register bits 255:224 (Read Only)
#define APIC_IRR1			0x0200 // Interrupt Request Register bits 31:0 (Read Only)
#define APIC_IRR2			0x0210 // Interrupt Request Register bits 63:32 (Read Only)
#define APIC_IRR3			0x0220 // Interrupt Request Register bits 95:64 (Read Only)
#define APIC_IRR4			0x0230 // Interrupt Request Register bits 127:96 (Read Only)
#define APIC_IRR5			0x0240 // Interrupt Request Register bits 159:128 (Read Only)
#define APIC_IRR6			0x0250 // Interrupt Request Register bits 191:160 (Read Only)
#define APIC_IRR7			0x0260 // Interrupt Request Register bits 223:192 (Read Only)
#define APIC_IRR8			0x0270 // Interrupt Request Register bits 255:224 (Read Only)
#define APIC_ESR			0x0280 // Error Status Register (Read Only)
#define APIC_LVT_CMCI		0x02F0 // LVT CMCI Register (Read/Write)
#define APIC_ICR1			0x0300 // Interrupt Command Register bits 0-31 (Read/Write)
#define APIC_ICR2			0x0310 // Interrupt Command Register bits 32-63 (Read/Write)
#define APIC_LVT_TIMER		0x0320 // LVT Timer Register (Read/Write)
#define APIC_LVT_THERMAL	0x0330 // LVT Thermal Sensor Register (Read/Write)
#define APIC_LVT_PMC		0x0340 // LVT Performance Monitoring Counters Register (Read/Write)
#define APIC_LVT_LINT0		0x0350 // LVT LINT0 Register (Read/Write)
#define APIC_LVT_LINT1		0x0360 // LVT LINT1 Register (Read/Write)
#define APIC_LVT_EREG		0x0370 // LVT Error Register (Read/Write)
#define APIC_INIT_COUNT		0x0380 // Initial Count Register (for Timer) (Read/Write)
#define APIC_CURR_COUNT		0x0390 // Current Count Register (for Timer) (Read Only)
#define APIC_DIV_CONF		0x03E0 // Divide Configuration Register (for Timer) (Read/Write)

//
// APIC entry types from ACPI MADT table
//

#define APIC_TYPE_LAPIC			0 // Local APIC
#define APIC_TYPE_IOAPIC		1 // IO APIC
#define APIC_TYPE_ISO			2 // Interrupt Service Override
#define APIC_TYPE_NMI			3 // Non-Maskable Interrupt Source
#define APIC_TYPE_LAPIC_NMI		4 // Local APIC NMI
#define APIC_TYPE_LAPIC_AO		5 // Local APIC Adress Override
#define APIC_TYPE_IOSAPIC		6 // IO SAPIC
#define APIC_TYPE_LSAPIC		7 // Local SAPIC
#define APIC_TYPE_PIS			8 // Platform Interrupt Sources
#define APIC_TYPE_Lx2APIC		9 // Local x2APIC
#define APIC_TYPE_Lx2APIC_NMI	10 // Local x2APIC NMI

/**
* APIC base MSR structure
*/
typedef union {
	struct {
		uint64 reserved1	: 8; // Reserved
		uint64 bsp			: 1; // Bootstrap processor
		uint64 reserved2	: 2; // Reserved
		uint64 enable		: 1; // Global APIC enable/disable bit
		uint64 base_addr	: 24; // APIC base address (4 KByte aligned)
		uint64 reserved		: 28; // Reserved
	} s;
	uint64 raw;
} apic_base_t;

/**
* Initialize APIC(s)
* @return true on success, false on failure
*/
bool apic_init();
/**
* Get APIC base address
* @return physical address of APIC memory maped registers
*/
apic_base_t apic_get_base();
/**
* Set APIC base address
* @param addr - physical address of APIC memory maped registers
*/
void apic_set_base(apic_base_t addr);
/**
* Read IOAPIC value
* @param addr - APIC base address
* @param reg - IOAPIC register selector
* @return data stored in register
*/
uint32 apic_read_ioapic(apic_base_t addr, uint32 reg);
/**
* Write IOAPIC value
* @param addr - APIC base address
* @param reg - IOAPIC register selector
* @param data - data to be stored in register
*/
void apic_write_ioapic(apic_base_t addr, uint32 reg, uint32 data);

#endif /* __apic_h */
