/*
 * io.h - Definició de l'entrada/sortida per pantalla en mode sistema
 */

#ifndef __IO_H__
#define __IO_H__

#include <types.h>

/** Screen functions **/
/**********************/

void printc(char c);
void printc_color(char c, Byte fg, Byte bg);
void printc_xy(Byte x, Byte y, char c, Byte fg, Byte bg);
void printk(char *string);
void printk_color(char *string, Byte fg, Byte bg);
void print_hexa(unsigned int v);
void welcome_screen(Byte fg, Byte bg);
void clear_screen(Byte color);

#endif  /* __IO_H__ */
