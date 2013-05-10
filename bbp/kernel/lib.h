/*

Tiny helper function library
============================

TOC:
	* Memory manipulation functions
	* Character string functions

Function argument order explanation. Basically it should sound like this:
"Fill this bucket for that many items of that other bucket", thus it's like
the Intel ASM syntax of "instruction destination, source" whith an extra size
specifier, so to prevent buffer overflows etc.

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

#ifndef __lib_h
#define __lib_h

#include "common.h"

#define INT_BASE_BIN 2
#define INT_BASE_OCT 8
#define INT_BASE_DEC 10
#define INT_BASE_HEX 16

//
// Memory manipulation functions
//

/**
* Copy data from one memory location to another
* @param [out] dest - destination memory
* @param [in] src - source memory
* @param len - number of bytes to copy
* @return void
*/
void mem_copy(uint8 *dest, uint64 len, const uint8 *src);
/**
* Fill a memory buffer with a single byte value
* @param [out] dest - destination memory
* @param [in] val - value to set
* @param len - number of bytes to set
* @return void
*/
void mem_fill(uint8 *dest, uint64 len, uint8 val);
/**
* Compare two memory regions
* @param [in] buff1
* @param [in] buff2
* @param len - comparison length
* @return true if they are equal, false if not
*/
bool mem_compare(const uint8 *buff1, const uint8 *buff2, uint64 len);

//
// String functions
//

/**
* Copy a string from one buffer to another
* @param [out] dest - destination buffer
* @param len - destination buffer length
* @param [in] src - source buffer
* @return number of characters copied
*/
uint64 str_copy(char *dest, uint64 len, const char *src);
/**
* Calculate the length of a string
* @param [in] str - string
* @return string length
*/
uint64 str_length(const char *str);
/**
* Search for a character in a string
* @param [in] haystack - source buffer
* @param needle - character to find
* @param offset - number of characters to offset from the beginning of haystack
* @return character index (-1 if not found)
*/
int64 str_char_idx(const char *haystack, const char needle, uint64 offset);
/**
* Convert integer to string
* @param [out] dest - destination buffer
* @param len - destination buffer length
* @param in - input integer
* @param base - the base of the integer represented by the string (8, 10 or 16)
* @return number of characters in a string representation
*/
uint64 int_to_str(char *dest, uint64 len, int64 val, int64 base);
/**
* Convert a string to integer
* @param [in] str - integer string
* @param base - the base of the integer represented by the string (8, 10 or 16)
* @return output integer
*/
int64 str_to_int(const char *str, int64 base);
/**
* Build a formated string from a format and additional parameters
* @param [out] dest - destination string pointer
* @param len - maximum length allowed to fill
* @param [in] format - standard C printf format string
* @param [in] ... - additional arguments
* @return number of characters written
*/
uint64 str_write_f(char *dest, uint64 len, const char *format, ...);
/**
* Parse a formated string using a format and store results in additional parameters
* @param [in] src - source string pointer
* @param [in] format - standard C printf format string
* @param [out] ... - additional argument pointers
* @return number of arguments parsed
*/
uint64 str_read_f(const char *src, const char *format, ...);

//
// Private functions
//

uint64 __write_f(char *dest, uint64 len, const char *format, va_list args);
uint64 __read_f(const char *src, const char *format, va_list args);


#endif /* __lib_h */