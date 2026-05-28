\#include <asm.h>

\#include <segment.h>



ENTRY (sys\_call\_table)

&#x09;.long sys\_ni\_syscall //0

&#x09;.long sys\_ni\_syscall //1

&#x09;.long sys\_fork //2

&#x09;.long sys\_ni\_syscall //3

&#x09;.long sys\_write //4

&#x09;.long sys\_ni\_syscall //5

&#x09;.long sys\_ni\_syscall //6

&#x09;.long sys\_ni\_syscall //7

&#x09;.long sys\_ni\_syscall //8

&#x09;.long sys\_ni\_syscall //9

&#x09;.long sys\_gettime //10

&#x09;.long sys\_ni\_syscall //11

&#x09;.long sys\_ni\_syscall //12

&#x09;.long sys\_ni\_syscall //13

&#x09;.long sys\_ni\_syscall //14

&#x09;.long sys\_write //15

&#x09;.long sys\_ni\_syscall //16

&#x09;.long sys\_ni\_syscall //17

&#x09;.long sys\_ni\_syscall //18

&#x09;.long sys\_ni\_syscall //19

&#x09;.long sys\_getpid //20

&#x09;#.long sys\_read // 21

&#x09;.long sys\_gotoxy //22

&#x09;#.long sys\_set\_color //23



.globl MAX\_SYSCALL

MAX\_SYSCALL = (. - sys\_call\_table)/4





\#include <libc.h>



char buff\[24];



int pid;



int \_\_attribute\_\_((\_\_section\_\_(".text.main")))

main(void)

{

&#x20; /\* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception \*/

&#x20; /\* \_\_asm\_\_ \_\_volatile\_\_ ("mov %0, %%cr3"::"r" (0) ); \*/



&#x20; /\* WRITE SYSCALL \*/

&#x20; char \*msg = "\\n La crida write funciona correctament des de mode usuari!\\n";

&#x20; if (write(1, msg, strlen(msg)) < 0) perror(); // test0 - tot correcte

&#x20; if (write(-2, msg, strlen(msg)) < 0) perror();  // test1 - descriptor de canal incorrecte

&#x20; if (write(1, msg, -63) < 0) perror();  // test2 - mida incorrecta

&#x20; if (write(1, (char\*)0x0, 10) < 0) perror();  // test3 - buffer null

&#x20; if (write(1, " ------------------------------\\n", 32) < 0) perror(); // separador

&#x20; 

&#x20; /\* GETTIME SYSCALL \*/

&#x20; char num\[12];

&#x20; itoa(gettime(), num); // crida a gettime() que es guarda com a string

&#x20; if (write(1, " El primer gettime retorna: ", 29) < 0) perror();

&#x20; if (write(1, num, strlen(num)) < 0) perror();

&#x20; for (int i = 0; i < 10000000; ++i) i = i+1; // sleep fictici

&#x20; itoa(gettime(), num);

&#x20; if (write(1, "\\n El segon gettime retorna: ", 29) < 0) perror();

&#x20; if (write(1, num, strlen(num)) < 0) perror();

&#x20; if (write(1, "\\n ------------------------------\\n", 33) < 0) perror(); // separador



&#x20; /\* PAGE FAULT EXCEPTION \*/

&#x20; //	char\* p = 0;

&#x20; //	p = 'x';



&#x20; /\* GETPID SYSCALL \*/

&#x20; itoa(getpid(), num);

&#x20; if (write(1, " getpid() retorna: ", 20) < 0) perror();

&#x20; if (write(1, num, strlen(num)) < 0) perror();

&#x20; 

&#x20; /\* FORK SYSCALL \*/

&#x20; //if (fork() < 0) perror();

&#x20; 



&#x20; //if(write(1, "\\n --Iniciant test gotoxy -- \\n", 29) < 0) perror();



&#x20; if(gotoxy(40,12) < 0) perror();

&#x20; write(1, "SOA FIB", 7);



&#x20; if(gotoxy(70,24) < 0) perror();

&#x20; write(1, "FINAL", 5);



&#x20; if(gotoxy(0,5) < 0) perror();

&#x20; write(1, "He tornat a la fila 5!", 22);



&#x20; if(gotoxy(100,50) < 0)

&#x20; {

&#x20;   write(1, "\\n Error de gotoxy detectat: ", 28);

&#x20;   perror();

&#x20; }



if (write(1, "\\n ------------------------------\\n", 33) < 0) perror();

&#x20; while (1)

&#x20; {

&#x20; }

}







\#include <asm.h>



ENTRY(write)

&#x20;   pushl %ebp

&#x20;   movl %esp, %ebp

&#x20;   pushl %ebx

&#x20;   pushl %esi

&#x20;   pushl %edi

&#x20;   movl $4, %eax

&#x20;   movl 0x8(%ebp), %edx 

&#x20;   movl 0xc(%ebp), %ecx

&#x20;   movl 0x10(%ebp), %ebx

&#x20;   pushl %ecx 

&#x20;   pushl %edx 

&#x20;   pushl $write\_ret

&#x20;   pushl %ebp

&#x20;   movl %esp, %ebp 

&#x20;   sysenter

write\_ret:

&#x20;   pop %ebp    

&#x20;   pop %edx    

&#x20;   pop %edx   

&#x20;   pop %ecx    

&#x20;   pop %edi

&#x20;   pop %esi

&#x20;   pop %ebx

&#x20;   test %eax, %eax

&#x20;   jns write\_end

&#x20;   negl %eax

&#x20;   movl %eax, errno

&#x20;   movl $-1, %eax

write\_end:

&#x20;   pop %ebp

&#x20;   ret



ENTRY(gettime)

&#x09;pushl %ebp

&#x09;movl %esp,%ebp

&#x09;pushl %edx

&#x09;pushl %ecx

&#x09;movl $10, %eax

&#x09;pushl $gettime\_ret

&#x09;pushl %ebp

&#x09;movl %esp,%ebp

&#x09;sysenter

gettime\_ret:

&#x09;pop %ebp

&#x09;addl $4, %esp

&#x09;pop %edx

&#x09;pop %ecx

&#x09;cmpl $0, %eax

&#x09;jge gettime\_end

&#x09;negl %eax 

&#x09;movl %eax, errno

&#x09;movl -1, %eax

gettime\_end:

&#x09;pop %ebp

&#x09;ret



ENTRY(getpid)

&#x09;pushl %ebp

&#x09;movl %esp,%ebp

&#x09;pushl %edx

&#x09;pushl %ecx

&#x09;movl $20, %eax

&#x09;pushl $getpid\_ret

&#x09;pushl %ebp

&#x09;movl %esp,%ebp

&#x09;sysenter

getpid\_ret:

&#x09;pop %ebp

&#x09;addl $4, %esp

&#x09;pop %edx

&#x09;pop %ecx

&#x09;cmpl $0, %eax

&#x09;jge getpid\_end

&#x09;negl %eax 

&#x09;movl %eax, errno

&#x09;movl -1, %eax

getpid\_end:

&#x09;pop %ebp

&#x09;ret





ENTRY(fork)

&#x09;pushl %ebp

&#x09;movl %esp,%ebp

&#x09;movl $0x2, %eax

&#x09;pushl %edx

&#x09;pushl %ecx

&#x09;push $fork\_ret

&#x09;pushl %ebp

&#x09;movl %esp,%ebp

&#x09;sysenter

fork\_ret:

&#x09;popl %ebp

&#x09;addl $4, %esp

&#x09;popl %edx

&#x09;popl %ecx

&#x09;cmpl $0, %eax

&#x09;jge fork\_no\_error

&#x09;negl %eax

&#x09;movl %eax, errno

&#x09;movl $-1, %eax

fork\_no\_error:

&#x09;popl %ebp

&#x09;ret



ENTRY(read)

&#x09;push %ebp 

&#x09;movl %esp, %ebp



&#x09;pushl %ebx;



&#x09;movl 8(%ebp), %ebx

&#x09;movl 12(%ebp), %esi

&#x09;movl $21, %eax



&#x09;pushl $.read\_return

&#x09;popl %edx

&#x09;movl %esp, %ecx 

&#x09;sysenter





.read\_return:

&#x09;cmpl $0, %eax 

&#x09;jge .no\_error

&#x09;negl %eax 

&#x09;movl %eax, errno

&#x09;movl $-1, %eax 



.no\_error:

&#x09;popl %ebx 

&#x09;popl %ebp 

&#x09;ret



ENTRY(gotoxy)

&#x09;push %ebp 

&#x09;movl %esp, %ebp



&#x09;pushl %ebx 

&#x09;pushl %esi 



&#x09;movl 8(%ebp), %ebx 

&#x09;movl 12(%ebp), %esi 



&#x09;movl $22, %eax 



&#x09;pushl $.Lgotoxy\_ret 

&#x09;popl %edx 

&#x09;movl %esp, %ecx 



&#x09;sysenter



.Lgotoxy\_ret:

&#x09;cmpl $0, %eax 

&#x09;jge .Lno\_error

&#x09;negl %eax 

&#x09;movl %eax, errno

&#x09;movl $-1, %eax 



.Lno\_error: 

&#x09;popl %esi 

&#x09;popl %ebx 

&#x09;popl %ebp

&#x09;ret









/\*

&#x20;\* sys.c - Syscalls implementation

&#x20;\*/

\#include <devices.h>



\#include <utils.h>



\#include <io.h>



\#include <mm.h>



\#include <mm\_address.h>



\#include <sched.h>



\#include <errno.h>



\#define LECTURA 0

\#define ESCRIPTURA 1



extern int zeos\_ticks;

extern Byte x;

extern Byte y;



int check\_fd(int fd, int permissions)

{

&#x20; if (fd != 1)

&#x20;   return -EBADF;

&#x20; if (permissions != ESCRIPTURA)

&#x20;   return -EACCES;

&#x20; return 0;

}



int sys\_ni\_syscall()

{

&#x20; return -ENOSYS;

}



int sys\_write(int fd, char \*buffer, int size)

{

&#x20; // comprovacions de paràmetres

&#x20; int err = check\_fd(fd, ESCRIPTURA);

&#x20; if (err < 0)

&#x20;   return err;

&#x20; if (buffer == NULL)

&#x20;   return -EFAULT;

&#x20; if (size < 0)

&#x20;   return -EINVAL;



&#x20; // va escrivint pel device byte a byte la cadena continguda a buffer

&#x20; int bytes\_written = 0;

&#x20; char kern\_byte;

&#x20; while (bytes\_written < size)

&#x20; {

&#x20;   if (copy\_from\_user(buffer, \&kern\_byte, 1) < 0)

&#x20;     return -EFAULT;



&#x20;   if (sys\_write\_console(\&kern\_byte, 1) < 0) 

&#x20;     return -EIO;



&#x20;   buffer++;

&#x20;   bytes\_written++;

&#x20; }



&#x20; return bytes\_written;

}



int sys\_gettime(void)

{

&#x20; return zeos\_ticks;

}



int sys\_getpid(void)

{

&#x20; return current()->PID;

}



int sys\_fork(void)

{

&#x20; return 0;

}



Circbuffer c;



int sys\_read(char\* b, int maxchars)

{



&#x20; int read\_count = 0;

&#x20; if(CIRCBUFFER\_MAX\_CAPACITY >= maxchars)

&#x20; {

&#x20;   return read\_count;

&#x20; }



&#x20; if(maxchars <= 0)

&#x20;   {

&#x20;     return -EINVAL;

&#x20;   }    



&#x20; struct task\_struct \*curr = current();



&#x20; // Bucle bloquejant si no tenim res al buffer

&#x20; while(isEmpty(c))

&#x20; {



&#x20; }





&#x20; while(read\_count < maxchars \&\& !isEmpty(c))

&#x20; {

&#x20;   char key = load(maxchars); // Obtenir tecla



&#x20;   // Copiar al user

&#x20;   if(copy\_to\_user(\&key, \&b\[read\_count], 1) < 0)

&#x20;   {

&#x20;     return -EFAULT;

&#x20;   }



&#x20;   ++read\_count;

&#x20; }



&#x20; return read\_count; // Retornem el nombre de tecles llegides

}









int sys\_gotoxy(int new\_x, int new\_y)

{

&#x20; if(new\_x < 0 || new\_x >= 80) return -EINVAL;

&#x20; if(new\_y < 0 || new\_y >= 25) return -EINVAL;



&#x20; x = (Byte)new\_x;

&#x20; y = (Byte)new\_y;



&#x20; return 0;

}



