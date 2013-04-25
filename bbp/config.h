/*

Global configuration
====================

This file contains global configuration used at compile-time.

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


#ifndef __config_h
#define __config_h

//
// Global settings
//

// Video mode
// 0 - none, leave it as is
// 1 - teletype
// 2 - VGA
#define VIDEOMODE 1
// Enable debug output
#define DEBUG 1
// Initial memory size to map to enter Long Mode
// we don't need more than this, but it should be more than 1MB
// as the PMLx structures will be located at the 1MB mark
#define INIT_MEM 0x200000 // 2MB
// Default page size
#define PAGE_SIZE 0x1000

//
// Hard-coded memory locations
//

// E820 memory map location
#define E820_LOC 0x0800
// Memory location where to store PMLx page tables
#define PT_LOC 0x00100000

#if VIDEOMODE == 1
	// Teletype video memory location
	// This can be used only in 32+ bit modes
	#define VIDEOMEM_LOC 0xB8000
#elif VIDEOMODE == 2
	// VGA video memory location
	// This can be used only in 32+ bit modes
	#define VIDEOMEM_LOC 0xA0000
#else
	// Null address
	#define VIDEOMEM_LOC 0x0
#endif

#endif
