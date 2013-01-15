/**
* @file memory.h
* Helper functions for operations on memory buffers
* @see memory.c
*/
#ifndef __memory_h
#define __memory_h

#include "common.h"

/**
* Copy data from one memory location to another
* @param [in] src - source memory
* @param [out] dest - destination memory
* @param len - number of bytes to copy
* @return void
*/
void mem_copy(const uint8 *src, uint8 *dest, uint32 len);
/**
* Set a value in memory
* @param [in] val - value to set
* @param [out] dest - destination memory
* @param len - number of bytes to set
* @return void
*/
void mem_set(uint8 val, uint8 *dest, uint32 len);
/**
* Compare two memory regions
* @param [in] buff1
* @param [in] buff2
* @param len - comparison length
* @return 1 if they are equal, 0 if not
*/
uint32 mem_cmp(const uint8 *buff1, const uint8 *buff2, uint32 len);

#endif
