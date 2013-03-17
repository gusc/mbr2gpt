Kernel
======

This is the Long Mode BBP kernel code

File list
---------

Main entry files:
* kmain.c - C entry point that get's called by ../boot/boot.asm

Helper files:
* acpi.c - ACPI table lokup implementation
* acpi.h - ACPI table structures
* common.h - data type definitions
* cpuid.h - inline assembly definition for CPUID instruction
* gdt.asm - GDT setup routines
* gdt.h - GDT C data structures
* idt.asm - IDT setup routines
* idt.h - IDT C data structures
* io.h - legaci IO instruction inline definitions
* interrupts.asm - interrupt service routines
* interrupts.h - intterupt service routine import in C
* screen.* - screen writing functions
* memory.* - memory functions
* msr.h - Model Specific Register (MSR) instructions inline definitions
* string.* - string functions

Build files:
* kernel.ld - Kernel linker script
* makefile - Make build script
