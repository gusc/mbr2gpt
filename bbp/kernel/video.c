/*

Helper functions for operations with teletype (text mode) screen
================================================================

Teletype video functions:
	* clear screen
	* print a formated string on the screen

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
#include "video.h"
#include "lib.h"

uint8 vcolumns = 80;
uint8 vrows = 25;
uint64 vrow_offset = 0;

void video_clear(uint8 color){
#if VIDEOMODE == 1
	char *vidmem = (char *)VIDEOMEM_LOC;
	uint16 i = 0;
	uint8 columns = 80;
	uint8 lines = 25;
	while (i < (columns * lines * 2)){
		vidmem[i] = ' ';
		i ++;
		vidmem[i] = color;
		i ++;
	}
#endif
}
void video_scroll(uint8 color){
#if VIDEOMODE == 1
	char *vidmem = (char *)VIDEOMEM_LOC;
	uint8 c;
	uint8 r;
	uint16 i;
	uint16 p;
	// Scroll the screen
	for (r = 1; r < vrows; r ++){
		for (c = 0; c < vcolumns; c ++){
			// Clear previous line
			p = ((r - 1) * vcolumns * 2) + (c * 2);
			vidmem[p] = ' ';
			vidmem[p + 1] = color;
			// Copy current line
			i = (r * vcolumns * 2) + (c * 2);
			vidmem[p] = vidmem[i];
			vidmem[p + 1] = vidmem[i + 1];
		}
	}
	// Clear the last line
	r = vrows - 1;
	for (c = 0; c < vcolumns; c ++){
		i = (r * vcolumns * 2) + (c * 2);
		vidmem[i] = ' ';
		vidmem[i + 1] = color;
	}
	vrow_offset ++;
#endif
}
void video_print(uint8 x, uint8 y, uint8 color, const char *format, ...){
#if VIDEOMODE == 1
	char *vidmem = (char *)VIDEOMEM_LOC;
	static char str[4000];
	char *s = (char *)format;
	mem_fill((uint8 *)str, 2000, 0);
	uint16 i;
	// Keep everything in bounds
	y = y - vrow_offset;
	//va_list args;
	//va_start(args, format);
	//if (__write_f(str, 2000, format, args)){
		// Write to screen only if there's any thing to write at all
		while (*s != 0){
			if (*s == 0x0A || x >= vcolumns){ // New line (a.k.a \n) or forced wrap
				x = 0;
				y ++;
			}
			if (y >= vrows){
				video_scroll(color);
				y = vrows - 1;
			}
			if (*s >= 0x20 && *s <= 0x7E){ // Only valid ASCII chars
				i = (y * vcolumns * 2) + (x * 2);
				vidmem[i] = *s;
				vidmem[i + 1] = color;
				x ++;
			}
			s++;
		}
	//}
	//va_end(args);
#endif
}
