/*

Helper functions for operations on memory buffers
=================================================

Memory buffer manipulation functions

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

#ifndef __memory_h
#define __memory_h

#include "common.h"

/**
* Copy data from one memory location to another
* @param [in] src - source memory
* @param [out] dest - destination memory
* @param len - number of bytes to copy
* @return void
*/
void mem_copy(const uint8 *src, uint8 *dest, uint32 len);
/**
* Set a value in memory
* @param [in] val - value to set
* @param [out] dest - destination memory
* @param len - number of bytes to set
* @return void
*/
void mem_set(uint8 val, uint8 *dest, uint32 len);
/**
* Compare two memory regions
* @param [in] buff1
* @param [in] buff2
* @param len - comparison length
* @return 1 if they are equal, 0 if not
*/
uint32 mem_cmp(const uint8 *buff1, const uint8 *buff2, uint32 len);

#endif
