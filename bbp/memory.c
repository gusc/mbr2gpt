#include "memory.h"

void mem_copy(const uint8 *src, uint8 *dest, uint32 len){
    while(len--){
		*dest++ = *src++;
	}
}

void mem_set(uint8 val, uint8 *dest, uint32 len){
    while(len--){
		*dest++ = val;
	}
}