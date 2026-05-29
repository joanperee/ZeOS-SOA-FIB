/*
 * libc.c
 */

#include <libc.h>
#include <errno.h>
#include <types.h>

int errno;

void itoa(int a, char *b)
{
  int i, i1;
  char c;

  if (a == 0)
  {
    b[0] = '0';
    b[1] = 0;
    return;
  }

  i = 0;
  while (a > 0)
  {
    b[i] = (a % 10) + '0';
    a = a / 10;
    i++;
  }

  for (i1 = 0; i1 < i / 2; i1++)
  {
    c = b[i1];
    b[i1] = b[i - i1 - 1];
    b[i - i1 - 1] = c;
  }
  b[i] = 0;
}

int strlen(char *a)
{
  int i;

  i = 0;

  while (a[i] != 0)
    i++;

  return i;
}

void perror(void)
{
  char *buff;
  static char tmp[32];
  switch (errno)
  {
  case ENOSYS:
    buff = " Syscall not implemented\n";
    break;
  case EFAULT:
    buff = " Bad address\n";
    break;
  case EINVAL:
    buff = " Invalid argument\n";
    break;
  case EACCES:
    buff = " Permission denied\n";
    break;
  case EBADF:
    buff = " Bad file descriptor\n";
    break;
  case EIO:
    buff = " I/O error\n";
    break;
  case ENOMEM:
    buff = " Out of memory\n";
    break;
  case EEXIST:
    buff = " File exists\n";
    break;
  case ENOENT:
    buff = " No such file or directory\n";
    break;
  case EMFILE:
    buff = " Too many open files\n";
    break;
  case ENFILE:
    buff = " File table overflow\n";
    break;
  case ENOTTY:
    buff = " Inappropriate ioctl for device\n";
    break;
  case EPERM:
    buff = " Operation not permitted\n";
    break;
  case ESRCH:
    buff = " No such process\n";
    break;
  case EINTR:
    buff = " Interrupted system call\n";
    break;
  default:
    itoa(errno, tmp);
    buff = tmp;
    break;
  }

  write(1, buff, strlen(buff));
}