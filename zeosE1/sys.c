/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <errno.h>

#define LECTURA 0
#define ESCRIPTURA 1

extern int zeos_ticks;

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
