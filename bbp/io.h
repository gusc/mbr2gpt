#ifndef __io_h
#define __io_h

#include "common.h"

/**
* Write a byte out to the specified port.
* @param uint16 - IO port number
* @param uint8 - byte value
* @return void
*/
inline void outb(uint16 port, uint8 value){
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}
/**
* Write a word out to the specified port.
* @param uint16 - IO port number
* @param uint16 - word value
* @return void
*/
inline void outw(uint16 port, uint16 value){
    asm volatile ("outw %1, %0" : : "dN" (port), "a" (value));
}
/**
* Read a byte from specified port.
* @param uint16 - IO port number
* @return uint8 - byte value
*/
inline uint8 inb(uint16 port){
    uint8 ret;
    asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}
/**
* Read a word from specified port.
* @param uint16 - IO port number
* @return uint16 - word value
*/
inline uint16 inw(uint16 port){
    uint16 ret;
    asm volatile("inw %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

#endif
