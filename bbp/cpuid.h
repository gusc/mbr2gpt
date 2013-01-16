/**
* @file cpuid.h
* CPUID wrapper
*/
#ifndef __cpuid_h
#define __cpuid_h

#include "common.h"

/**
* Read CPUID
* @param type - initial EAX value (information type to get from CPUID)
* @param [out] eax - EAX value returned by CPUID
* @param [out] ebx - EBX value returned by CPUID
* @param [out] ecx - ECX value returned by CPUID
* @param [out] edx - EDX value returned by CPUID
* @return void
*/
inline void cpuid(uint32 type, uint32 *eax, uint32 *ebx, uint32 *ecx, uint32 *edx){
   asm volatile("cpuid" : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx) : "a"(type));
}

#endif
