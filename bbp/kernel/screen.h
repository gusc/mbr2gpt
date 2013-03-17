/*

Helper functions for operations with teletype (text mode) screen
================================================================

Teletype video functions:
	* clear screen
	* put a string on the screen
	* put an integer on the screen

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

#ifndef __screen_h
#define __screen_h

#include "common.h"

/**
* Clear the teletype (text mode) screen
* @param color - color byte
* @return void
*/
void screen_clear(uint8 color);
/**
* Print a string on the teletype (text mode) screen
* @param [in] message - text message
* @param color - color byte
* @param x coordinate (a.k.a. column 0-79)
* @param y coordinate (a.k.a. line 0-24)
* @return void
*/
void screen_print_str(const char *message, uint8 color, uint8 x, uint8 y);
/**
* Print an integer on the teletype (text mode) screen
* @param [in] value - integer value
* @param color - color byte
* @param x coordinate (a.k.a. column 0-79)
* @param y coordinate (a.k.a. line 0-24)
* @return void
*/
void screen_print_int(const uint32 value, uint8 color, uint8 x, uint8 y);

#endif
