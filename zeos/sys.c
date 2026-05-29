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
}

void* sys_shmat(int id, void* addr)
{
    // 1. Validar el ID
    if (id < 0 || id >= NUM_SHARED_PAGES) return (void*) -EINVAL;

    unsigned int addr_page;
    page_table_entry *proc_tp = get_PT(current());


    if (addr == NULL)
    {
        addr_page = get_user_free_page(current());
        if (addr_page == -1) return (void*) -ENOMEM;
    }
    else
    
        if (((unsigned int) addr) % PAGE_SIZE != 0) return (void*) -EINVAL;    

        unsigned int req_page = ((unsigned int) addr) >> 12;
        

        if (req_page < 1024) return (void*) -EINVAL; 

        addr_page = req_page - 1024; 
        
        if (!pagina_logica_free(proc_tp, addr_page))
        {
            addr_page = get_user_free_page(current());
            if (addr_page == -1) return (void*) -ENOMEM;
        }
    }

    if (shm_table[id].frame == -1) 
    {
        int new_frame = alloc_frame();
        if (new_frame == -1) return (void*) -ENOMEM;
        shm_table[id].frame = new_frame;
    }

    set_ss_pag(proc_tp, addr_page, shm_table[id].frame, 1);
    

    set_cr3(get_DIR(current()));

    shm_table[id].refs++;
    
    return (void *) ((1024 + addr_page) << 12);
}