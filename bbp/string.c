#include "string.h"
#include "memory.h"

#define MAX_INT_STR 20

uint32 str_copy(const char *src, char *dest, uint32 len){
	char *sp = (char *)src;
	char *dp = (char *)dest;
	uint32 s = 0;
	while (*sp != 0 && len != 0){
		// Copy till the end of source or destination has been reached
		*(dp++) = *(sp++);
		len --;
		s ++;
	}
	return s;
}

uint32 int_to_str(int32 val, char *dest, uint32 len){
	static char tmp[MAX_INT_STR + 1];
	mem_set(0, (uint8 *)tmp, MAX_INT_STR + 1);
	char *b = (char *)tmp + MAX_INT_STR;
	if (val >= 0){
		// Process positive value
		do {
			*--b = '0' + (val % 10);
			val /= 10;
		} while (val != 0);
	} else if (val < 0){
		// Process negative value
		do {
			*--b = '0' - (val % 10);
			val /= 10;
		} while (val != 0);
		*--b = '-';
	}
	// Copy the result and return number of digits processed
	return str_copy(b, dest, len);
}

int32 str_to_int(const char *src){
	char *b = (char *)src;
	int32 val = 0;
	uint8 negative = 0;
	// Trim leading spaces
	while (*b == 0x20 || *b == 0x09){ // space || tab
		b++;
	}
	// Check if a negative number
	if (*b == '-'){
		negative = 1;
	}
	// Calculate integer value
	while (*b >= 0x30 && *b <= 0x39){
		val *= 10; // multiply upwwards
		val += (*b) - 0x30; // add current digit
		b ++;
	}
	// Set minus sign if necessary
	if (negative){
		return 0 - val;
	}
	return val;
}

uint32 str_len(const char *src){
	uint32 len = 0;
	while (*(src++) != 0){
		// Increment till the end of string
		len ++;
	}
	return len;
}

int32 str_char_pos(const char *haystack, const char needle, uint32 offset){
	int32 idx = 0;
	// Move offset
	while (offset--){
		if (*(haystack++) == 0){
			// End of the string
			return -1;
		}
	}
	while (*haystack != 0){
		if (*haystack == needle){
			// Found an occurence
			return idx;
		}
		idx++;
		haystack++;
	}
	return -1;
}
