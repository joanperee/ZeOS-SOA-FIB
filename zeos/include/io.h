/*
 * io.h - Definició de l'entrada/sortida per pantalla en mode sistema
 */

#ifndef __IO_H__
#define __IO_H__

#include <types.h>

/** Screen functions **/
/**********************/

// Imprimeix el caràcter c amb color de lletra fg i fons bg
void printc_color(char c, Byte fg, Byte bg);

// Imprimeix la cadena apuntada per string amb color de lletra fg i fons bg
void printk_color(char *string, Byte fg, Byte bg);

// Pantalla de benvinguda amb color de lletra fg i fons bg,
// on s'inclou informació dels autors i del sistema operatiu
void welcome_screen(Byte fg, Byte bg);

// Neteja la pantalla i la posa buida d'un color
// (internament imprimeix molts punts "." amb fg = bg fins omplir-la tota)
void clear_screen(Byte color);

// Imprimeix el caràcter c amb color de lletra fg i fons bg a la posició (x,y)
void printc_xy(Byte x, Byte y, char c, Byte fg, Byte bg);

// Valors per defecte de printc_color amb fg = WHITE i bg = BLACK
void printc(char c);

// Valors per defecte de printk_color amb fg = WHITE i bg = BLACK
void printk(char *string);

// Funció auxiliar que passa un integer a hexadecimal
void print_hexa(unsigned int v);


#endif  /* __IO_H__ */
