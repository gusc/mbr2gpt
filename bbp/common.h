/*

Common data types
=================

This file contains type definitions and helper structures.

The idea of this file is to keep a precise control over data type sizes, so 
instead of int and long, we should have int32 or int64.

Maybe in the future we'll introduce memory pointer type intptr or simply ptr,
that will reference the largest integer type for memory access of the machine.
That is, a 64bit int for x86_64 architecture and 32bit int for x86.

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

#ifndef __common_h
#define __common_h

// Default types
typedef unsigned char	uchar;

typedef unsigned char	uint8;
typedef unsigned short	uint16;
typedef unsigned int	uint32;
typedef unsigned long	uint64;

typedef char			int8;
typedef short			int16;
typedef int				int32;
typedef long			int64;

typedef float			float32;
typedef double			float64;

typedef struct {
	char *buff;
	uint64 len;
} string;

typedef void *handle_t;
#define null 0

// Packed types
typedef union {
	struct {
		uint8 low;
		uint8 high;
	} byte;
	uint16 word;
} pack16;
typedef union {
	struct {
		uint16 low;
		uint16 high;
	} word;
	uint32 dword;
} pack32;
typedef union {
	struct {
		uint32 low;
		uint32 high;
	} dword;
	uint64 qword;
} pack64;

// Packed array
typedef union {
	uint8 byte[8];
	uint16 word[4];
	uint32 dword[2];
	uint64 qword;
} pack64_arr;

#endif /* __common_h */
