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

uint32 mem_cmp(const uint8 *buff1, const uint8 *buff2, uint32 len){
	while (len--){
		if (*(buff1++) != *(buff2++)){
			return false;
		}
	}
	return true;
}
