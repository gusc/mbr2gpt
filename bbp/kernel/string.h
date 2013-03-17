/*

Helper functions for operations with string buffers
===================================================

Basic ASCII string manipulation functions.

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

#ifndef __string_h
#define __string_h

#include "common.h"

/**
* Copy a string from one buffer to another
* @param [in] src - source buffer
* @param [out] dest - destination buffer
* @param len - destination buffer length
* @return number of characters copied
*/
uint32 str_copy(const char *src, char *dest, uint32 len);
/**
* Calculate the length of a string
* @param [in] src - source buffer
* @return string length
*/
uint32 str_len(const char *src);
/**
* Search for a character in a string
* @param [in] haystack - source buffer
* @param needle - character to find
* @param offset - number of characters to offset from the beginning of haystack
* @return character index (-1 if not found)
*/
int32 str_char_pos(const char *haystack, const char needle, uint32 offset);
/**
* Convert integer to string
* @param in - input integer
* @param [out] dest - destination buffer
* @param len - destination buffer length
* @return number of characters in a string representation
*/
uint32 int_to_str(int32 in, char *dest, uint32 len);
/**
* Convert a string to integer
* @param [in] src - source buffer
* @return output integer
*/
int32 str_to_int(const char *src);


#endif
