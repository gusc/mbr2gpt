mbr2gpt
=======

MBR bootcode to read GPT headers and load bootloader from BIOS Boot Partition

mbr
---

Master Boot Record code

bbp
---

Test BIOS Boot Partition code

Requirements
------------

You'll need:
* MinGW Cross-compiler that's capable of building ELF files. See how to build one here: http://gusc.lv/2012/12/myos-mingw-cross-compiler/
* Netwide Assembler from: http://www.nasm.us/
* DiskUtils from: https://github.com/gusc/diskutils
* Microsoft Visual Studio Express 2010
