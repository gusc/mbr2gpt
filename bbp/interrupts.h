#ifndef __interrupts_h
#define __interrupts_h

#include "common.h"

typedef struct registers
{
    uint32 ds;										// Data segment selector
    uint32 edi, esi, ebp, esp, ebx, edx, ecx, eax;	// Pushed by pusha.
    uint32 int_no, err_code;						// Interrupt number and error code (if applicable)
    uint32 eip, cs, eflags, useresp, ss;			// Pushed by the processor automatically.
} registers_t;

extern void int_service_routine0();
extern void int_service_routine1();
extern void int_service_routine2();
extern void int_service_routine3();
extern void int_service_routine4();
extern void int_service_routine5();
extern void int_service_routine6();
extern void int_service_routine7();
extern void int_service_routine8();
extern void int_service_routine9();
extern void int_service_routine10();
extern void int_service_routine11();
extern void int_service_routine12();
extern void int_service_routine13();
extern void int_service_routine14();
extern void int_service_routine15();
extern void int_service_routine16();
extern void int_service_routine17();
extern void int_service_routine18();
extern void int_service_routine19();
extern void int_service_routine20();
extern void int_service_routine21();
extern void int_service_routine22();
extern void int_service_routine23();
extern void int_service_routine24();
extern void int_service_routine25();
extern void int_service_routine26();
extern void int_service_routine27();
extern void int_service_routine28();
extern void int_service_routine29();
extern void int_service_routine30();
extern void int_service_routine31();

#endif