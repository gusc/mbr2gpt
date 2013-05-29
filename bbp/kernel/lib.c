/*

Tiny helper function library
============================

TOC:
	* Memory manipulation functions
	* Character string functions

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

#include "lib.h"
#include "common.h"

//
// Memory manipulation functions
//

void mem_copy(uint8 *dest, uint64 len, const uint8 *src){
	while(len--){
		*dest++ = *src++;
	}
}
void mem_fill(uint8 *dest, uint64 len, uint8 val){
	while(len--){
		*dest++ = val;
	}
}
bool mem_compare(const uint8 *buff1, const uint8 *buff2, uint64 len){
	while (len--){
		if (*(buff1++) != *(buff2++)){
			return false;
		}
	}
	return true;
}

//
// String functions
//

#define MAX_INT_STR 32

uint64 str_copy(char *dest, uint64 len, const char *src){
	char *sp = (char *)src;
	char *dp = (char *)dest;
	uint64 s = 0;
	while (*sp != 0 && len != 0){
		// Copy till the end of source or destination has been reached
		*(dp++) = *(sp++);
		len --;
		s ++;
	}
	return s;
}
uint64 str_length(const char *str){
	uint64 len = 0;
	while (*(str++) != 0){
		// Increment till the end of string
		len ++;
	}
	return len;
}
int64 str_char_idx(const char *haystack, const char needle, uint64 offset){
	int64 idx = 0;
	// Move offset
	while (offset--){
		if (*(haystack++) == 0){
			// End of the string
			return -1;
		}
	}
	while (*haystack != 0){
		if (*haystack == needle){
			// Found an occurence
			return idx;
		}
		idx++;
		haystack++;
	}
	return -1;
}
uint64 int_to_str(char *dest, uint64 len, int64 val, int64 base){
	static char tmp[MAX_INT_STR + 1];
	mem_fill((uint8 *)tmp, MAX_INT_STR + 1, 0);
	char *b = (char *)tmp + MAX_INT_STR;
	char c = 0;
	if (val >= 0){
		// Process positive value
		do {
			c = (char)(val % base);
			if (c > 9){
				*--b = 'A' + (c - 10);
			} else {
				*--b = '0' + c;
			}
			val /= base;
		} while (val != 0);
	} else if (val < 0){
		// Process negative value
		do {
			c = (char)(val % base);
			if (c < -9){
				*--b = 'A' - (c + 10);
			} else {
				*--b = '0' - c;
			}
			val /= base;
		} while (val != 0);
		// Add minus sign only for decimals
		if (base == 10){
			*--b = '-';
		}
	}
	// Copy the result and return number of digits processed
	return str_copy(dest, len, b);
}
int64 str_to_int(const char *str, int64 base){
	char *b = (char *)str;
	int64 val = 0;
	uint8 negative = 0;
	// Trim leading spaces
	while (*b == 0x20 || *b == 0x09){ // space || tab
		b++;
	}
	// Check if a negative number
	if (*b == '-'){
		negative = 1;
	}
	// Calculate integer value
	while (*b >= 0x30 && *b <= 0x39){ // >= 0 && <= 9 (as in ascii 0, 1, ..., 9)
		val *= 10; // multiply upwwards
		val += (*b) - 0x30; // add current digit
		b ++;
	}
	// Set minus sign if necessary
	if (negative){
		return 0 - val;
	}
	return val;
}
uint64 str_write_f(char *dest, uint64 len, const char *format, ...){
	va_list args;
	va_start(args, format);
	uint64 ret = __write_f(dest, len, format, args);
	va_end(args);
	return ret;
}
uint64 str_read_f(const char *src, const char *format, ...){
	va_list args;
	va_start(args, format);
	uint64 ret = __read_f(src, format, args);
	va_end(args);
	return ret;
}


//
// Private functions
//

uint64 __write_f(char *dest, uint64 len, const char *format, va_list args){
	char *f = (char *)format;
	char *d = (char *)dest;
	uint64 ret = 0;
	
	// Temporary value variables
	uint64 val_len;
	int64 val_int64;
	uint64 val_uint64;
	char val_char;
	char *val_str;
	
	while (*f && ret < len){
		if (*f == '%'){ // possible specifier
			switch (*(f + 1)){
				case 'd': // integer
					val_int64 = va_arg(args, int64);
					val_len = int_to_str(d, MAX_INT_STR, val_int64, 10); 
					d += val_len;
					ret += val_len;
					f ++;
					break;
				case 'u': // unsigned integer
					val_uint64 = va_arg(args, uint64);
					val_len = int_to_str(d, MAX_INT_STR, val_uint64, 10); 
					d += val_len;
					ret += val_len;
					f ++;
					break;
				case 'x': // unsigned integer in hex
					val_uint64 = va_arg(args, uint64);
					val_len = int_to_str(d, MAX_INT_STR, val_uint64, 16); 
					d += val_len;
					ret += val_len;
					f ++;
					break;
				case 'c': // char
					val_char = (char)va_arg(args, int64); // chars are promoted to int in va_list
					*(d++) = val_char;
					ret ++;
					f ++;
					break;
				case 's': // string
					val_str = (char *)va_arg(args, uint64);
					while (*val_str){
						*(d ++) = *(val_str ++);
						ret ++;
					}
					f ++;
					break;
				default: // not a specifier
					*(d ++) = *f;
					ret ++;
					break;
			}
			f ++;
		} else { // copy string
			*(d ++) = *(f ++);
			ret ++;
		}
	}
	return ret;
}
uint64 __read_f(const char *src, const char *format, va_list args){

	return 0;
}