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
#include "debug_print.h"
#include "lib.h"

// Video screen size
static uint64 _columns = 80;
static uint64 _rows = 25;
// Global cursor
static uint64 _x = 0;
static uint64 _y = 0;
static uint8 _base_color = 0x00;

static void __debug_print_f(uint8 x, uint8 y, uint8 color, const char *format, va_list args);

void debug_clear(uint8 color){
	char *vidmem = (char *)VIDEOMEM_LOC;
	_base_color = color;
	_y = 0;
	_x = 0;
	uint16 fill = (_base_color << 8) + ' ';
	// Fast clear line
	asm volatile ("rep\n\tstosw" : : "a"(fill), "c"(_columns * _rows), "D"(vidmem));
}

void debug_scroll(){
	char *vidmem = (char *)VIDEOMEM_LOC;
	uint16 row_len = _columns * 2;
	uint16 last_row_char = (_rows - 1) * _columns;
	uint16 fill = (_base_color << 8) + ' ';
	// Fast scroll
	asm volatile ("rep\n\tmovsw" : : "c"(last_row_char), "S"(vidmem + row_len), "D"(vidmem));
	// Fast clear line
	asm volatile ("rep\n\tstosw" : : "a"(fill), "c"(_columns), "D"(vidmem + (last_row_char * 2)));
}

void debug_print_at(uint8 x, uint8 y, uint8 color, const char *format, ...){
	va_list args;
	va_start(args, format);
	__debug_print_f(x, y, color, format, args);
	va_end(args);
}

void debug_print(uint8 color, const char *format, ...){
	if (_y >= _rows){
		debug_scroll();
		_y = _rows - 1;
	}
	va_list args;
	va_start(args, format);
	__debug_print_f(_x, _y, color, format, args);
	va_end(args);
	_y ++;
}

static void __debug_print_f(uint8 x, uint8 y, uint8 color, const char *format, va_list args){
	char *vidmem = (char *)VIDEOMEM_LOC;
	static char str[2001];
	mem_fill((uint8 *)str, 2001, 0);
	uint16 i;
	// Keep everything in bounds
	if (__write_f(str, 2000, format, args)){
		char *s = (char *)str;
		while (*s != 0){
			if (*s == 0x0A || x >= _columns){ // New line (a.k.a \n) or forced wrap
				x = 0;
				y ++;
				if (y >= _rows){
					debug_scroll();
					y = _rows - 1;
				}
			}
			if (*s >= 0x20 && *s <= 0x7E){ // Only valid ASCII chars
				i = (y * _columns * 2) + (x * 2);
				vidmem[i] = *s;
				vidmem[i + 1] = color;
				x ++;
			}
			s++;
		}
	}
}