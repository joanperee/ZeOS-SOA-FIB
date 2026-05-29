/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>
#include <hardware.h>
#include <circbuffer.h>

char initial_stack[KERNEL_STACK_SIZE]; // Space for the initial system stack
struct task_struct *init_task;         // easily retrieve INIT task struct
struct task_struct *idle_task;         // easily retrieve IDLE task struct
struct list_head freequeue;
struct list_head readyqueue;
struct list_head keyboard_blocked_queue;
int DIRspt;
page_table_entry *DirSPTaddr;
Circbuffer c;

// =======================================================================
// NUEVO: MEMORIA ESTÁTICA PARA EL SISTEMA (10 procesos máximos)
// =======================================================================
#define NR_TASKS 10
union task_union task[NR_TASKS] __attribute__((__section__(".data.task")));
page_table_entry dir_pages[NR_TASKS][1024] __attribute__((aligned(4096)));
page_table_entry pag_pages[NR_TASKS][1024] __attribute__((aligned(4096)));

// NUEVO: Función nativa para asignar directorios sin romper la TLB
void allocate_DIR(struct task_struct *t) 
{
    int pos = ((union task_union *)t - task); // Índice del array (0 a 9)
    
    page_table_entry *dir = &dir_pages[pos][0];
    page_table_entry *pt = &pag_pages[pos][0];
    
    clear_page_table(dir);
    clear_page_table(pt);

    // Entrada 0: Kernel
    set_ss_pag(dir, 0, DIRspt, 0);

    // Entrada 1: Usuario (obtenemos el frame físico de la tabla)
    int pt_frame = ((unsigned int)pt) >> 12;
    set_ss_pag(dir, 1, pt_frame, 1);

    t->dir_pages_baseAddr = dir;
}
// =======================================================================

void cpu_idle(void)
{
	__sti();
	while (1)
	{
		;
	}
}

void init_idle(void)
{
	int DIR = alloc_frame();
	page_table_entry *DIRaddr = (page_table_entry *)(DIR << 12);
	clear_page_table(DIRaddr);
	set_ss_pag(DIRaddr, 0, DIRspt, 0);

	int idle_page = alloc_frame();
	set_ss_pag(DirSPTaddr, idle_page, idle_page, 0);
	union task_union *idle_union = (union task_union *)(idle_page << 12);
	idle_task = (struct task_struct *)idle_union;

	idle_union->stack[KERNEL_STACK_SIZE - 1] = (unsigned long)&cpu_idle;
	idle_union->stack[KERNEL_STACK_SIZE - 2] = 0;

	idle_task->PID = 0;
	idle_task->dir_pages_baseAddr = DIRaddr;
	idle_task->kernel_esp = (unsigned long)&(idle_union->stack[KERNEL_STACK_SIZE - 2]);
}

void init_task1(void)
{
	// Taula de pàgines del directori arrel
	int DIR = alloc_frame();
	page_table_entry *DIRaddr = (page_table_entry *)(DIR << 12);
	clear_page_table(DIRaddr);

	// Taula de pàgines per la memòria del SO (system page table) GLOBAL
	DIRspt = alloc_frame();
	set_ss_pag(DIRaddr, 0, DIRspt, 0);
	DirSPTaddr = (page_table_entry *)(DIRspt << 12);
	clear_page_table(DirSPTaddr);
	set_kernel_pages(DirSPTaddr);

	// Taula de pàgines per la memòria d'usuari (init és el 1r proc que entra en mode user)
	int DIRusr = alloc_frame();
	set_ss_pag(DIRaddr, 1, DIRusr, 1);
	page_table_entry *DirUSRaddr = (page_table_entry *)(DIRusr << 12);
	clear_page_table(DirUSRaddr);
	set_user_pages(DirUSRaddr);

	// Dins del directori de SO, hem d'afegir els 3 directoris per a que IDlogic = IDfisic
	set_ss_pag(DirSPTaddr, DIR, DIR, 0);
	set_ss_pag(DirSPTaddr, DIRspt, DIRspt, 0);
	set_ss_pag(DirSPTaddr, DIRusr, DIRusr, 1);

	// Assignem una nova union, que serà indexada a 1 en el vector de processos "task"
	// i obtenim el seu task_struct
	int init_page = alloc_frame();
	set_ss_pag(DirSPTaddr, init_page, init_page, 0);

	union task_union *init_union = (union task_union *)(init_page << 12);
	init_task = (struct task_struct *)init_union;

	init_task->PID = 1;
	init_task->dir_pages_baseAddr = DIRaddr;

	// el camp ESP0 del registre TSS ha d'apuntar a la primera direcció de
	// la pila de sistema, que és l'última posició de la union. A més, usem
	// SYSENTER, per tant, hem d'actualitzar el registre 0x175 amb writeMSR
	// que conté la direcció de ESP
	tss.esp0 = (unsigned long)&(init_union->stack[KERNEL_STACK_SIZE]);
	writeMSR(0x175, tss.esp0);

	// actualitzar cr3 per a que apunti al nou directori d'INIT
	set_cr3(init_task->dir_pages_baseAddr);
}

void init_sched()
{
	// AÑADIDO: Inicializar la freequeue que faltaba
	INIT_LIST_HEAD(&freequeue);
	INIT_LIST_HEAD(&readyqueue);
	INIT_LIST_HEAD(&keyboard_blocked_queue);
	start_circbuffer(&c);

	// AÑADIDO: Llenamos la freequeue con los 10 PCBs estáticos legales
	for (int i = 0; i < NR_TASKS; i++) {
		task[i].task.PID = -1;
		list_add_tail(&(task[i].task.list), &freequeue);
	}
}

void inner_task_switch(union task_union *newtask)
{
	// el camp esp0 de la TSS apuntarà a la pila de sistema nova (també s'actualitza l'MSR)
	tss.esp0 = (unsigned long)&(newtask->stack[KERNEL_STACK_SIZE]);
	writeMSR(0x175, tss.esp0);

	// s'actualitza el cr3 per a que contengui la direcció del directori de newtask
	set_cr3(get_DIR(&(newtask->task)));

	// salvar el valor d'EBP al procés actual per quan es torni a executar (x86-asm)
	update_ebp_esp(&(current()->kernel_esp), newtask->task.kernel_esp);
}

void sched_next(void)
{
	struct list_head *l;
	struct task_struct *next_task;

	if (!list_empty(&readyqueue))
	{
		l = list_first(&readyqueue);
		list_del(l);
		next_task = list_head_to_task_struct(l);
	}
	else
	{
		next_task = idle_task;
	}

	next_task->state = ST_RUN;
	inner_task_switch((union task_union *)next_task);
}

/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry *get_DIR(struct task_struct *t)
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry *get_PT(struct task_struct *t)
{
    // AÑADIMOS EL [1] PARA ACCEDER A LA TABLA DE USUARIO
    return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr[1].bits.pbase_addr)) << 12);
}