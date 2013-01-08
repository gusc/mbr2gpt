#ifndef __string_h
#define __string_h

#include "common.h"

/**
* Copy a string from one buffer to another
* @param const char *[in] - source buffer
* @param char *[out] - destination buffer
* @param uint32 - destination buffer length
* @return uint32 - number of characters copied
*/
uint32 str_copy(const char *src, char *dest, uint32 len);
/**
* Calculate the length of a string
* @param const char *[in] - source buffer
* @return uint32 - string length
*/
uint32 str_len(const char *src);
/**
* Search for a character in a string
* @param const char *[in] - source buffer
* @param const char - character to find
* @param uint32 - number of characters to offset from the beginning of haystack
* @return int32 - character index (-1 if not found)
*/
int32 str_char_pos(const char *haystack, const char needle, uint32 offset);
/**
* Convert integer to string
* @param int32 - input integer
* @param char *[out] - destination buffer
* @param uint32 - destination buffer length
* @return uint32 - number of characters in a string representation
*/
uint32 int_to_str(int32 in, char *dest, uint32 len);
/**
* Convert a string to integer
* @param const char *[in] - source buffer
* @return int32 - output integer
*/
int32 str_to_int(const char *src);


#endif
