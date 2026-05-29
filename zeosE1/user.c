#include <libc.h>

char buff[24];

int pid;

int __attribute__((__section__(".text.main")))
main(void)
{
  /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
  /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  /* WRITE SYSCALL */
  char *msg = "\n La crida write funciona correctament des de mode usuari!\n";
  if (write(1, msg, strlen(msg)) < 0) perror(); // test0 - tot correcte
  if (write(-2, msg, strlen(msg)) < 0) perror();  // test1 - descriptor de canal incorrecte
  if (write(1, msg, -63) < 0) perror();  // test2 - mida incorrecta
  if (write(1, (char*)0x0, 10) < 0) perror();  // test3 - buffer null
  if (write(1, " ------------------------------\n", 32) < 0) perror(); // separador
  
  /* GETTIME SYSCALL */
  char num[12];
  itoa(gettime(), num); // crida a gettime() que es guarda com a string
  if (write(1, " El primer gettime retorna: ", 29) < 0) perror();
  if (write(1, num, strlen(num)) < 0) perror();
  for (int i = 0; i < 10000000; ++i) i = i+1; // sleep fictici
  itoa(gettime(), num);
  if (write(1, "\n El segon gettime retorna: ", 29) < 0) perror();
  if (write(1, num, strlen(num)) < 0) perror();
  if (write(1, "\n ------------------------------\n", 33) < 0) perror(); // separador

  /* PAGE FAULT EXCEPTION (descomentar per provocar-la) */
  // char* p = 0;
  // *p = 'x';

  while (1)
  {
  }
}
