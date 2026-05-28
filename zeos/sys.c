/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <libc.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <errno.h>

#include <circbuffer.h>

#include <colors.h>

#define LECTURA 0
#define ESCRIPTURA 1

extern int zeos_ticks;

extern Circbuffer c;

int check_fd(int fd, int permissions)
{
  if (fd != 1)
    return -EBADF;
  if (permissions != ESCRIPTURA)
    return -EACCES;
  return 0;
}

int sys_ni_syscall()
{
  return -ENOSYS;
}

int sys_write(int fd, char *buffer, int size)
{
  // comprovacions de paràmetres
  int err = check_fd(fd, ESCRIPTURA);
  if (err < 0)
    return err;
  if (buffer == NULL)
    return -EFAULT;
  if (size < 0)
    return -EINVAL;

  // va escrivint pel device byte a byte la cadena continguda a buffer
  int bytes_written = 0;
  char kern_byte;
  while (bytes_written < size)
  {
    if (copy_from_user(buffer, &kern_byte, 1) < 0)
      return -EFAULT;

    if (sys_write_console(&kern_byte, 1) < 0)
      return -EIO;

    buffer++;
    bytes_written++;
  }

  return bytes_written;
}

int sys_gettime(void)
{
  return zeos_ticks;
}

// MILESTONE 3: reuse existing syscall to check allocatable frames
int sys_getpid(void)
{
  // See how many phyisical allocated frames we have initially
  char buf[4];
  for (int i = 0; i < TOTAL_PAGES; ++i)
    if (phys_mem[i] == 1) {
      itoa(i, buf);
      printk(" "); printk(buf);
    }

  // See how many phyisical frames we can allocate
  int allocated_frames = 0;
  while (alloc_frame() != -1) {
    allocated_frames++;
  }

  // Reset all frames to continue executing ZeOS
  init_frames();  
  
  return allocated_frames;
}

int sys_read(char *b, int maxchars)
{
  if (maxchars < 0)
    return -EINVAL;
  if (!access_ok(VERIFY_WRITE, b, maxchars))
    return -EFAULT;

  while (c.size < maxchars)
  {
    current()->state = ST_BLOCKED;
    list_add_tail(&(current()->list), &keyboard_blocked_queue);
    sched_next();
  }

  char temp_buff[CIRCBUFFER_MAX_CAPACITY];
  int bytes_to_load = (maxchars < CIRCBUFFER_MAX_CAPACITY) ? maxchars : CIRCBUFFER_MAX_CAPACITY;
  int actual_read = load(&c, temp_buff, bytes_to_load);

  if (actual_read > 0)
    copy_to_user(temp_buff, b, actual_read);

  return actual_read;
}

//MILESTONE 4
int sys_gotoxy(int x, int y)
{
  if(x < 0 || x >= 80 || y < 0 || y >= 25)
  {
    return -EINVAL;
  }

  kernel_gotoxy(x,y);

  return 0;
}

int sys_set_color(int fg, int bg)
{
  if(fg > WHITE || fg < BLACK || bg > WHITE || bg < BLACK)
  {
    return -EINVAL;
  }

  kernel_set_color(fg, bg);

  return 0;
}

int sys_fork(void)
{
  return -1;
  // int i;
  // // 1. Allotjar frame per al task_union del fill (Pila sistema + task_struct)
  // int child_union_frame = alloc_frame();
  // if (child_union_frame == -1)
  //   return -ENOMEM;

  // // Calculem les adreces virtuals (mapeig 1:1 al kernel)
  // union task_union *child_union = (union task_union *)(child_union_frame << 12);
  // union task_union *parent_union = (union task_union *)current();
  // struct task_struct *child_pcb = &(child_union->task);

  // // 2. Copiar el task_union sencer (PCB + Pila de sistema del pare)
  // copy_data(parent_union, child_union, sizeof(union task_union));

  // // 3. Allotjar i configurar el Directori de Pàgines (DIR) del fill
  // int dir_frame = alloc_frame();
  // if (dir_frame == -1)
  // {
  //   free_frame(child_union_frame);
  //   return -ENOMEM;
  // }
  // child_pcb->dir_pages_baseAddr = (page_table_entry *)(dir_frame << 12);

  // clear_page_table(child_pcb->dir_pages_baseAddr);

  // // Copiem la taula de pàgines de sistema (DIRspt és la global del sistema)
  // // DirSPTaddr és un punter, passem el seu frame físic
  // set_ss_pag(child_pcb->dir_pages_baseAddr, 0, (unsigned int)DirSPTaddr >> 12, 0);

  // // 4. Gestionar l'espai d'usuari
  // page_table_entry *child_PT = get_PT(child_pcb);
  // page_table_entry *parent_PT = get_PT(current());

  // // CODI: Es comparteix el mateix frame físic (només lectura)
  // for (i = 0; i < NUM_PAG_CODE; i++)
  // {
  //   int page = PAG_LOG_INIT_CODE + i;
  //   int frame = get_frame(parent_PT, page);
  //   set_ss_pag(child_PT, page, frame, 0);
  // }

  // // DADES: Copia física en frames nous
  // for (i = 0; i < NUM_PAG_DATA; i++)
  // {
  //   int page = PAG_LOG_INIT_DATA + i;
  //   int new_frame = alloc_frame();
  //   if (new_frame == -1)
  //   {
  //     /* En un SO real hauries d'alliberar tots els frames de dades demanats abans */
  //     return -ENOMEM;
  //   }

  //   // Mapegem frame nou al fill
  //   set_ss_pag(child_PT, page, new_frame, 1);

  //   // Truc per copiar: mapegem el frame del fill en una pàgina lliure del pare
  //   // Usem la pàgina següent a les de dades com a "finestra" temporal
  //   int temp_page = PAG_LOG_INIT_DATA + NUM_PAG_DATA + i;
  //   set_ss_pag(parent_PT, temp_page, new_frame, 1);

  //   copy_data((void *)(page << 12), (void *)(temp_page << 12), PAGE_SIZE);

  //   del_ss_pag(parent_PT, temp_page);
  // }

  // // Refresquem TLB
  // set_cr3(get_DIR(current()));

  // // 5. Assignar PID i estat
  // child_pcb->PID = 666;

  // // 6. Preparar la pila del fill per al task_switch
  // // Necessitem que el kernel_esp del fill apunti on el pare té el seu ESP ara mateix
  // unsigned long esp_actual;
  // __asm__ __volatile__("movl %%esp, %0" : "=r"(esp_actual));

  // // Distància des de l'inici de la pila del pare
  // unsigned long offset = esp_actual - (unsigned long)parent_union;
  // // Apliquem la mateixa distància a la pila del fill
  // child_pcb->kernel_esp = (unsigned long)child_union + offset;

  // // 7. Modificar el valor de retorn del fill a 0
  // // L'offset 0x18 correspon a la posició d'EAX desada pel SAVE_ALL a entry.S
  // // un cop s'ha entrat via sysenter.
  // *((unsigned long *)(child_pcb->kernel_esp + 0x18)) = 0;

  // // 8. Posar el fill a la cua de preparats
  // list_add_tail(&(child_pcb->list), &readyqueue);

  // // El pare retorna el PID del fill
  // return child_pcb->PID;
}