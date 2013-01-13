#include "screen.h"
#include "string.h"
#include "memory.h"

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

void screen_print_str(const char *message, uint8 color, uint8 x, uint8 y){
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

void screen_print_int(const uint32 value, uint8 color, uint8 x, uint8 y){
	static char integer[21] = "";
	mem_set(0, (uint8 *)integer, 21);
	int_to_str(value, integer, 20);
	screen_print_str(integer, color, x, y);
}
