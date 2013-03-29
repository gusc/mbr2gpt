Kernel
======

This is the Long Mode BBP kernel code

File list
---------

Main entry files:
* kmain.c - C entry point that get's called by ../boot/boot.asm

Helper files:
* acpi.* - ACPI table lokup implementation
* common.h - data type definitions
* cpuid.h - inline assembly definition for CPUID instruction
* interrupts.asm - interrupt service routines
* interrupts.h - intterupt service routine import in C
* io.h - legacy IO instruction inline definitions
* lib.* - tiny C helper library
* msr.h - Model Specific Register (MSR) instructions inline definitions
* pci.* - PCI operation functions
* video.* - Video output functions

Build files:
* kernel.ld - Kernel linker script
* makefile - Make build script
