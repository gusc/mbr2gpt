Boot
====

This is the main bootstrap code

File list
---------

Main entry files:
* boot.asm - This file is compiled at the top of BBP binary image and receives control from MBR

Helper files:
* bios.asm - BIOS wrapper routines used by main16.c
* common16.h - Real Mode data types (also directs GCC to emmit 16bit assembly)
* common32.h - Protected Mode data types
* main16.* - Real Mode initialization code
* main32.* - Protected Mode initialization code

Build files:
* boot.ld - Bootstrap linker script
* makefile - Make build script
