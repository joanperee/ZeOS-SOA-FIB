/*
 * interrupt.h - Definició de les diferents rutines de tractament d'exepcions
 */

#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <types.h>

#define IDT_ENTRIES 256

extern Gate idt[IDT_ENTRIES];
extern Register idtR;

void setInterruptHandler(int vector, void (*handler)(), int maxAccessibleFromPL);
void setTrapHandler(int vector, void (*handler)(), int maxAccessibleFromPL);

void setIdt();

// Funcions implementades en assemblador (handlers)
void keyboard_handler(void);
void clock_handler(void);
void syscall_handler_sysenter(void);
void writeMSR(long num, long val);
void my_page_fault_handler(void);

// Funcions implementades en C (rutines)
void keyboard_routine(void);
void clock_routine(void);
void my_page_fault_routine(int error_code, int eip);



#endif  /* __INTERRUPT_H__ */
