#include "screen.h"

static const uint8 columns = 80;
static const uint8 lines = 25;

void screen_clear(uint8 color){
	char *vidmem = (char *) 0xb8000; // mapped vido memory location
	uint16 i = 0;
	while(i < (columns * lines  *2)){
		vidmem[i] = 0x20; // Space
		i ++;
		vidmem[i] = color;
		i ++;
	}
}

void screen_print(const char *message, uint8 color, uint8 x, uint8 y){
	char *vidmem = (char *) 0xb8000; // mapped vido memory location
	uint16 i = (y * columns * 2) + (x * 2);
	while(*message != 0){
		if(*message == 0x0A){ // New line (a.k.a \n)
			y ++;
			i = (y * columns * 2);
			*message ++;
		} else {
			vidmem[i] = *message;
			*message ++;
			i ++;
			vidmem[i] = color;
			i ++;
		}
	}
}
