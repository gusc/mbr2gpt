/**
* @file msr.h
* Helper functions for Model Specific Register (MSR) operations
*/
#ifndef __msr_h
#define __msr_h

#include "common.h"

// MSR macros
#define MSR_IA32_P5_MC_ADDR 0x0
#define MSR_IA32_P5_MC_TYPE 0x1
#define MSR_IA32_APIC_BASE 0x1B
#define MSR_IA32_FEATURE_CONTROL 0x3A
#define MSR_BBL_CR_CTL3 0x11E
#define MSR_IA32_MISC_ENABLE 0x1A0
#define MSR_IA32_EFER 0xC0000080
#define MSR_IA32_STAR 0xC0000081

/**
* Read MSR value
* @param msr - Model Specific Register
* @param [out] lo - low dword
* @param [out] hi - high dword
* @return void
*/
inline void msr_read(uint32 msr, uint32 *lo, uint32 *hi){
   asm volatile("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}
/**
* Write MSR value
* @param msr - Model Specific Register
* @param [in] lo - low dword
* @param [in] hi - high dword
* @return void
*/
inline void msr_write(uint32 msr, uint32 lo, uint32 hi){
   asm volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}

#endif
