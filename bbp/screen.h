#ifndef __screen_h
#define __screen_h

#include "common.h"

/**
* Clear the teletype (text mode) screen
* @param uint8 - color byte
* @return void
*/
void screen_clear(uint8 color);
/**
* Print a string on the teletype (text mode) screen
* @param const char *[in] - text message
* @param uint8 - color byte
* @param uint16 - x coordinate (a.k.a. column 0-79)
* @param uint16 - y coordinate (a.k.a. line 0-24)
* @return void
*/
void screen_print(const char *message, uint8 color, uint8 x, uint8 y);

#endif
