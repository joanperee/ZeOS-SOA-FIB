/*
 * sched.h - Estructures i macros pel tractament de processos
 */

#ifndef __SCHED_H__
#define __SCHED_H__

#include <list.h>
#include <types.h>
#include <mm_address.h>

#define KERNEL_STACK_SIZE 1024
#define NUM_TASKS 10
#define KERNEL_ESP(t)       	(DWord) &(t)->stack[KERNEL_STACK_SIZE]
#define INITIAL_ESP             (DWord) &initial_stack[KERNEL_STACK_SIZE]

extern char initial_stack[KERNEL_STACK_SIZE];
extern page_table_entry *DirSPTaddr; /* El directori de sistema serà accessible per sys_fork */
extern struct list_head freequeue;
extern struct list_head readyqueue;

extern struct list_head keyboard_blocked_queue; // Cua per els procesos que es bloquejen esperant el teclat.


#define list_head_to_task_struct(listptr) ((struct task_struct*)((void*)(listptr) - (unsigned int)&(((struct task_struct*)0)->list)))

enum state_t { ST_RUN, ST_READY, ST_BLOCKED };

// PCB d'un procés
struct task_struct {
  int PID;			/* Process ID. This MUST be the first field of the struct. */
  page_table_entry * dir_pages_baseAddr;

  enum state_t state;

  unsigned long kernel_esp; // guarda el l'ebp de la pila de sistema pel canvi de ctx
  struct list_head list;
};

// Estructura que comprèn la PCB i la pila de sistema d'un procés en format union
union task_union {
  struct task_struct task;
  unsigned long stack[KERNEL_STACK_SIZE];    /* pila de sistema, per procés */
};

// Inicialitza les dades del procés inicial INIT, el primer que s'executa i que passa a mode usuari
void init_task1(void);

// Inicialitza les dades del procés IDLE, que s'executarà si l'scheduler no té cap procés per assignar a la CPU
void init_idle(void);

// De moment està buida
void init_sched(void);

// Funció definida en asm x86-32 que prepara el canvi de context de current a newtask
void task_switch(union task_union *newtask);

// Funció en C que rep newtask i executa el canvi de context
void inner_task_switch(union task_union *newtask);

void sched_next(void);

void update_ebp_esp(unsigned long *save_here, unsigned long esp);

// Retorna el task_struct (PCB) del procés actual, current()
struct task_struct * current();

// Funció definida en asm x86-32 (com totes les altres, a entry.S)
// que actualitza el registre MSR num amb el valor val
void writeMSR(long num, long val);

page_table_entry * get_PT (struct task_struct *t) ;

page_table_entry * get_DIR (struct task_struct *t) ;


#endif  /* __SCHED_H__ */