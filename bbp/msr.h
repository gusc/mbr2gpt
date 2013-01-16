/**
* @file msr.h
* Helper functions for Model Specific Register (MSR) operations
*/
#ifndef __msr_h
#define __msr_h

#include "common.h"

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
