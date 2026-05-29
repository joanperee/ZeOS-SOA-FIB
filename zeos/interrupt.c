#include <types.h>
#include <interrupt.h>
#include <segment.h>
#include <hardware.h>
#include <io.h>
#include <colors.h>
#include <circbuffer.h>
#include <zeos_interrupt.h>
#include <sched.h>
 
Gate idt[IDT_ENTRIES];
Register idtR;
 
int zeos_ticks = 0;
int need_sched = 0;   /* flag per fer reschedule després del RESTORE_ALL/iret */
 
extern Circbuffer c;
extern struct task_struct *idle_task;
 
unsigned char char_map[] = {
    '\0', '\0', '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '\'', 0xA1, '\0', '\0',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', '`', '+', '\0', '\0', 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', 0xF1,
    '\0', 0xBA, '\0', 0xE7, 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '-', '\0', '*',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '7',
    '8', '9', '-', '4', '5', '6', '+', '1',
    '2', '3', '0', '\0', '\0', '\0', '<', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0'
};
 
void setInterruptHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  Word flags = (Word)(maxAccessibleFromPL << 13);
  flags |= 0x8E00; /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */
 
  idt[vector].lowOffset = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags = flags;
  idt[vector].highOffset = highWord((DWord)handler);
}
 
void setTrapHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  Word flags = (Word)(maxAccessibleFromPL << 13);
  flags |= 0x8E00; /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */
 
  idt[vector].lowOffset = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags = flags;
  idt[vector].highOffset = highWord((DWord)handler);
}
 
void setIdt()
{
  idtR.base = (DWord)idt;
  idtR.limit = IDT_ENTRIES * sizeof(Gate) - 1;
 
  set_handlers();
 
  setInterruptHandler(33, keyboard_handler, 0);
  setInterruptHandler(32, clock_handler, 0);
  setInterruptHandler(14, my_page_fault_handler, 0);
 
  writeMSR(0x174, __KERNEL_CS);
  writeMSR(0x175, INITIAL_ESP);
  writeMSR(0x176, (long)syscall_handler_sysenter);
 
  set_idt_reg(&idtR);
}
 
void keyboard_routine(void)
{
  Byte fg = BLACK;
  Byte bg = GREEN;
  unsigned char portlecture = inb(0x60);
  int isMake = portlecture & 0x80;
 
  if (isMake == 0)  /* make: bit7=0 = tecla presionada */
  {
    unsigned char keyout = char_map[portlecture & 0x7F];
    if (keyout == '\0') keyout = 'C';
 
    int x = 65;
    int y = 20;
 
    printc_xy(x - 1, y - 1, ' ', fg, bg);
    printc_xy(x, y - 1, ' ', fg, bg);
    printc_xy(x + 1, y - 1, ' ', fg, bg);
    printc_xy(x - 1, y, ' ', fg, bg);
    printc_xy(x + 1, y, ' ', fg, bg);
    printc_xy(x - 1, y + 1, ' ', fg, bg);
    printc_xy(x, y + 1, ' ', fg, bg);
    printc_xy(x + 1, y + 1, ' ', fg, bg);
    printc_xy(x, y, keyout, fg, bg);
 
    store(&c, keyout);
 
    if (!list_empty(&keyboard_blocked_queue))
    {
      struct list_head *l = list_first(&keyboard_blocked_queue);
      list_del(l);
      struct task_struct *t = list_head_to_task_struct(l);
      t->state = ST_READY;
      list_add_tail(l, &readyqueue);
 
      /* Si estem a idle, marcar que cal fer reschedule després del iret */
      if (current() == (struct task_struct*)idle_task)
        need_sched = 1;
    }
  }
}
 
void clock_routine(void)
{
  zeos_ticks++;
  zeos_show_clock();
 
  /* Si hi ha processos llestos i estem a idle, marcar reschedule */
  if (!list_empty(&readyqueue) && current() == (struct task_struct*)idle_task)
    need_sched = 1;
}
 
void my_page_fault_routine(int error_code, int eip)
{
  printk("\n Excepcio de PAGE FAULT generada a EIP 0x");
  print_hexa(eip);
  while(1);
}