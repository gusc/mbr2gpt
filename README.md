mbr2gpt
=======

MBR bootcode to read GPT headers and load bootloader from BIOS Boot Partition

mbr
---

Master Boot Record code

bbp
---

Test BIOS Boot Partition code (my playground to write an OS)

Virtual Machines
----------------

* MBR2GPT.vbox - VirtualBox test machine
* MBR2GPT.vmx - VMWare Player test machine
* run_bochs.bat - launch Bochs test machine
* run_bochs_debug.bat - launch Bochs test machine with GUI debugger
* run_qemu.bat - launch QEMU test machine

Other files
-----------

* bochsrc.bxrc - Bochs configuration file
* bochsrc_debug.bxrc - Bochs configuration file (with GUI debugger)
* Disk.vmdk - VMDK disk image header for VirtualBox and VMWare Player test machines

Build Requirements
------------------

You'll need:
* MinGW Cross-compiler that's capable of building ELF files. See how to build one here: http://gusc.lv/2012/12/myos-mingw-cross-compiler/
* Netwide Assembler from: http://www.nasm.us/
* DiskUtils from: https://github.com/gusc/diskutils
* Microsoft Visual Studio Express 2010
