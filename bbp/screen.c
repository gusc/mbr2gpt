#include "screen.h"

void screen_clear(uint8 color){
	char *vidmem = (char *) 0xb8000;
	unsigned int i = 0;
	while(i < (80*25*2)){
		vidmem[i] = ' ';
		i++;
		vidmem[i] = color;
		i++;
	}
}

void screen_print(char *message, uint8 color, uint32 line){
	char *vidmem = (char *) 0xb8000;
	uint32 i=0;

	i=(line*80*2);

	while(*message != 0){
		if(*message == '\n'){
			line++;
			i = (line*80*2);
			*message++;
		} else {
			vidmem[i] = *message;
			*message++;
			i++;
			vidmem[i] = color;
			i++;
		}
	}
}
