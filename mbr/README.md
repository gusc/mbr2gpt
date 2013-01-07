Master Boot Record
==================

This is the MBR bootcode that read GPT headers and partition array and loads first 512KiB of BIOS Boot Partition bootcode

File list
---------

* src/main.asm - main MBR code
* src/main.inc - data structure definitions and memory location definitions
* mbr.vcxproj - Visual Studio 2010 project file


Requirements
------------

* Netwide Assembler from: http://www.nasm.us/
* Microsoft Visual Studio Express 2010
