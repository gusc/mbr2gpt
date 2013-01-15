/**
* @file screen.h
* Helper functions for operations with teletype (text mode) screen
* @see screen.c
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
