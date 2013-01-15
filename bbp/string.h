/**
* @file string.h
* Helper functions for operations with string buffers
* @see string.c
*/
#ifndef __string_h
#define __string_h

#include "common.h"

/**
* Copy a string from one buffer to another
* @param [in] src - source buffer
* @param [out] dest - destination buffer
* @param len - destination buffer length
* @return number of characters copied
*/
uint32 str_copy(const char *src, char *dest, uint32 len);
/**
* Calculate the length of a string
* @param [in] src - source buffer
* @return string length
*/
uint32 str_len(const char *src);
/**
* Search for a character in a string
* @param [in] haystack - source buffer
* @param needle - character to find
* @param offset - number of characters to offset from the beginning of haystack
* @return character index (-1 if not found)
*/
int32 str_char_pos(const char *haystack, const char needle, uint32 offset);
/**
* Convert integer to string
* @param in - input integer
* @param [out] dest - destination buffer
* @param len - destination buffer length
* @return number of characters in a string representation
*/
uint32 int_to_str(int32 in, char *dest, uint32 len);
/**
* Convert a string to integer
* @param [in] src - source buffer
* @return output integer
*/
int32 str_to_int(const char *src);


#endif
