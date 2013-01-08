#ifndef __screen_h
#define __screen_h

#include "common.h"

void screen_clear(uint8 color);
void screen_print(char *message, uint8 color, uint32 line);

#endif
