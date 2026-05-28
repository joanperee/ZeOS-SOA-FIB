/*
 * libc.h - macros per fer els traps amb diferents arguments
 *          definició de les crides a sistema
 */
 
#ifndef __LIBC_H__
#define __LIBC_H__

// Passa l'integer "a" a un buffer apuntat per "b"
void itoa(int a, char *b);

// Retorna la mida del buffer apuntat per "a"
int strlen(char *a);

// Escriu per pantalla l'error que ha causat la fallada
void perror(void);

// Crida a sistema que escriu per pantalla amb el dispositiu sys_write_console
int write(int fd, char *buffer, int size);

// Crida a sistema que retorna els ticks que han passat des del boot
int gettime(void);

// Crida a sistema que retorna el PID del procés actual current()
int getpid(void);

// Crida a sistema que genera un procés fill i hi copia les dades del pare
int fork(void);

// Crida a sistema que llegeix del circular buffer 
int read(char *b, int maxchars);

#endif  /* __LIBC_H__ */
