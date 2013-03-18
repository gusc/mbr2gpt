/*

Common data types (long mode)
=============================

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

#define __NOINLINE  __attribute__((noinline))
#define __INLINE	__attribute__((always_inline))
#define __REGPARM   __attribute__((regparm(3)))
#define __NORETURN  __attribute__((noreturn))
#define __PACKED	__attribute__((packed))
#define __ALIGN(x)	__attribute__((aligned(x)))

#define BREAK() asm volatile ("xchg %bx, %bx")
#define HANG() while(true){}

// Default types
typedef unsigned char	uchar;

typedef unsigned char	uint8;
typedef unsigned short	uint16;
typedef unsigned int	uint32;
typedef unsigned long long uint64;

typedef char			int8;
typedef short			int16;
typedef int				int32;
typedef long long		int64;

#define null 0
#define true 1
#define false 0
#define bool uint64

#endif /* __common_h */
