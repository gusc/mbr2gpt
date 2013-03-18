/*

Real Mode initialization
=============================

This file contains initialization code real mode (preparation to switch to 
protected mode).

It does:
	* video mode switch
	* memory mapping

License (BSD-3)
===============

Copyright (c) 2013, Gusts 'gusC' Kaksis <gusts.kaksis@gmail.com>
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

#include "main16.h"
#include "../config.h"

/**
* Set video mode
* @see bios.asm
* @param mode - BIOS video mode
*/
extern void set_video_mode(uint16 mode);
/**
* Set video mode SuperVGA
* @see bios.asm
* @param mode - SuperVGA video mode
*/
extern void set_svga_mode(uint16 mode);
/**
* Enable A20 gate
* @see bios.asm
*/
extern void enable_a20();
/**
* Read E820 memory map
* @see bios.asm
* @param mem_map - e820 memory map structure pointer
* @return status code (1 - ok, 0 - failed)
*/
extern uint32 read_e820(e820map_t *mem_map);
/**
* Filter out memory map of unusable regions
* and sort them in ascending order
*/
static void filter_e820(e820map_t *mem_map){
	uint16 i = 0;
	// Filter out unusuable regions
	while (i < mem_map->size){
		if (mem_map->entries[i].length < 0 || mem_map->entries[i].type == kMemBad){
			if (i + 1 < mem_map->size){
				mem_map->entries[i] = mem_map->entries[i + 1];
			}
			mem_map->size --;
		} else {
			i ++;
		}
	}
	// Do the bubble sort to make them in ascending order
	e820entry_t e;
	uint16 swapped  = 1;
	uint16 count = mem_map->size;
	while (count > 0 && swapped ){
		i = 0;
		swapped  = 0;
		while (i < count - 1){
			if (mem_map->entries[i].base > mem_map->entries[i + 1].base){
				e = mem_map->entries[i];
				mem_map->entries[i] = mem_map->entries[i + 1];
				mem_map->entries[i + 1] = e;
				swapped  = 1;
			}
			i ++;
		}
		count --;
	}
	RET32();
}

/**
* Initialize Real Mode
*/
void main16(){
	e820map_t *mem_map = (e820map_t *)E820_LOC;
#if VIDEOMODE == 1
	set_video_mode(0x03); // Teletype
#elif VIDEOMODE == 2
	set_svga_mode(0x011B); // 1280x1024 (24 bit) 
#endif
	// Enable A20 gate
	enable_a20();
	// Read E820 map
	if (read_e820(mem_map)){
		// Process E820 map
		filter_e820(mem_map);
	} else {
		HANG();
	}	

	// Exit like we want it!
	// It's uggly, but as we can not control the entry of this function
	// at least we can control the exit. 
	RET32();
}
