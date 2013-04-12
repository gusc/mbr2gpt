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

#ifndef __video_h
#define __video_h

#include "common.h"

// Some fancy color definitions :)
#define DC_WB 0xF0
#define DC_BW 0x0F
#define DC_WLG 0xF7
#define DC_WDG 0xF8
#define DC_WBL 0xF1
#define DC_WGR 0xF2
#define DC_WRD 0xF4

/**
* Clear the teletype (text mode) screen
* @param color - color byte
* @return void
*/
void debug_clear(uint8 color);
/**
* Scroll whole video buffer upwards
* @return void
*/
void debug_scroll();
/**
* Print a formated string on the teletype (text mode) screen
* @param x coordinate (a.k.a. column 0-79)
* @param y coordinate (a.k.a. line 0-24)
* @param color - color byte
* @param [in] format - standard C printf format string
* @param [in] ... - additional arguments
* @return void
*/
void debug_print_at(uint8 x, uint8 y, uint8 color, const char *format, ...);
/**
* Print a formated string on the teletype (text mode) screen
* @param color - color byte
* @param [in] format - standard C printf format string
* @param [in] ... - additional arguments
* @return void
*/
void debug_print(uint8 color, const char *format, ...);

#endif /* __video_h */
