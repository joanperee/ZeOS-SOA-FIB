#include <libc.h>
#include <colors.h>

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
  // if (write(-2, msg, strlen(msg)) < 0) perror();  // test1 - descriptor de canal incorrecte
  // if (write(1, msg, -63) < 0) perror();  // test2 - mida incorrecta
  // if (write(1, (char*)0x0, 10) < 0) perror();  // test3 - buffer null
  // if (write(1, " ------------------------------\n", 32) < 0) perror(); // separador
  
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

  /* PAGE FAULT EXCEPTION */
  //	char* p = 0;
  //	p = 'x';

  /* GETPID SYSCALL ALREADY TESTED*/
  // MILESTONE 3: getpid() syscall reformatted to debug the memory expansion
  
  // UNIMPLEMENTED or INCORRECT: fork(), schedule(), exit(), block(), unblock()

  /* READ SYSCALL */
  if (write(1, " Read syscall ALREADY TESTED", 29) < 0) perror();
  // char b[5];  // buffer for the test
  // int n = read(b, 5); // we block the process to wait until it reads 5 chars into buffer b
  // if (n >= 0) write(1, b, n); // write all characters read
  // else write(1, " Read error\n", 12);  // error
  // if (write(1, "\n Press 5 more keys: ", 22) < 0) perror();
  // int n2 = read(b, 5); // we block the process to wait until it reads 5 chars into buffer b
  // if (n2 >= 0) write(1, b, n2); // write all characters read
  // else write(1, " Read error\n", 12);  // error
  if (write(1, "\n ------------------------------\n", 33) < 0) perror(); // separador

  /** MILESTONE 3: EXPAND MEM TO 2048 PHYSICAL FRAMEs */
  char frames[4];
  write(1, " These frames are allocated initially (x62):\n", 44);
  itoa(getpid(), frames);
  write(1, "\n\n Quantity of frames allocatable: ", 36);
  write(1, frames, 4);
  write(1, "\n Total: 62 + 1986  = 2048\n", 28);

  if(gotoxy(40, 21) >= 0)
  {
    write(1, "GOTOXY(40,20)\n", 14);
  }

  else
  {
    perror();
  }

  set_color(GREEN,BLUE);
  write(1, "\nText en verd amb fons blau!\n", 28);

  set_color(WHITE, BLACK);
  write(1, "\nTornem al blanc i negre normal.\n",32);

  while (1)
  {
  }
}
