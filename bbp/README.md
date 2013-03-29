BIOS Boot Partition
===================

This is the BBP bootcode that get's loaded by MBR.

File list
---------

Main entry files:
* boot/boot.asm - main BBP entry code (real mode code)
* boot/ - Real Mode -> Protected Mode -> Long Mode bootstrap code
* kernel/ - Kernel code

Build files:
* bbp.ld - BBP linker script
* bbp.vcxproj - Visual Studio 2010 project file
* buildimg.bat - Bach script to merge MBR and BBP code into Bochs disk image file using DiskUtils
* config.h - global configuration file (compile-time configuration)
* makefile - Make build script


Requirements
------------

* MinGW Cross-compiler that's capable of building ELF64 files. See how to build one here: http://gusc.lv/2012/12/myos-mingw-cross-compiler/
* Netwide Assembler from: http://www.nasm.us/
* DiskUtils from: https://github.com/gusc/diskutils
* Microsoft Visual Studio Express 2010
