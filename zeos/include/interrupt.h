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

// Handler en assemblador de la interrupció de teclat
void keyboard_handler(void);

// Handler en assemblador de la interrupció de rellotge
void clock_handler(void);

// Handler del punt d'entrada al sistema amb SYSENTER
void syscall_handler_sysenter(void);

// Funció que actualitza algun dels registres MSR 
void writeMSR(long num, long val);

// Handler de l'excepció page_fault
void my_page_fault_handler(void);

// Rutina en C per la interrupció de teclat 
void keyboard_routine(void);

// Rutina en C per la interrupció de rellotge
void clock_routine(void);

// Rutina en C per l'excepció de fallada de pàgina 
void my_page_fault_routine(int error_code, int eip);


#endif  /* __INTERRUPT_H__ */
