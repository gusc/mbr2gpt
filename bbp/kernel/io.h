/*

Helper functions for legacy IO operations
=========================================

ASM wrappers

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

#ifndef __io_h
#define __io_h

#include "common.h"

/**
* Write a byte out to the specified port.
* @param port - IO port number
* @param value - byte value
* @return void
*/
static void outb(uint16 port, uint8 value){
	asm volatile ("outb %1, %0" : : "d"(port), "a"(value));
}
/**
* Write a word out to the specified port.
* @param port - IO port number
* @param value - word value
* @return void
*/
static void outw(uint16 port, uint16 value){
	asm volatile ("outw %1, %0" : : "d"(port), "a"(value));
}
/**
* Write a dword out to the specified port.
* @param port - IO port number
* @param value - dword value
* @return void
*/
static void outd(uint16 port, uint32 value){
	asm volatile ("outl %1, %0" : : "d"(port), "a"(value));
}
/**
* Read a byte from specified port.
* @param port - IO port number
* @return byte value
*/
static uint8 inb(uint16 port){
	uint8 ret;
	asm volatile("inb %1, %0" : "=a"(ret) : "d"(port));
	return ret;
}
/**
* Read a word from specified port.
* @param port - IO port number
* @return word value
*/
static uint16 inw(uint16 port){
	uint16 ret;
	asm volatile("inw %1, %0" : "=a"(ret) : "d"(port));
	return ret;
}
/**
* Read a dword from specified port.
* @param port - IO port number
* @return dword value
*/
static uint32 ind(uint16 port){
	uint32 ret;
	asm volatile("inl %1, %0" : "=a"(ret) : "d"(port));
	return ret;
}

#endif
