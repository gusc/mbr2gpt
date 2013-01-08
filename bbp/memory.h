#ifndef __memory_h
#define __memory_h

#include "common.h"

/**
* Copy data from one memory location to another
* @param const uint8 *[in] - source memory
* @param uint8 *[out] - destination memory
* @param uint32 - number of bytes to copy
* @return void
*/
void mem_copy(const uint8 *src, uint8 *dest, uint32 len);
/**
* Set a value in memory
* @param uint8 [in] - value to set
* @param uint8 *[out] - destination memory
* @param uint32 - number of bytes to set
* @return void
*/
void mem_set(uint8 val, uint8 *dest, uint32 len);

#endif
