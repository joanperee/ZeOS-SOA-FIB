/*
 * io.c -
 */

#include <io.h>
#include <types.h>
#include <hardware.h>
#include <colors.h>

/**************/
/** Screen  ***/
/**************/

#define NUM_COLUMNS 80
#define NUM_ROWS 25

Byte x, y = 19;

// Per evitar refactoritzar tot el codi, mantenim la funció printc
// però com a wrapper de la funció printc_color usant paràmetres
// per defecte, és a dir, fg = WHITE, bg = BLACK
void printc(char c)
{
  printc_color(c, WHITE, BLACK);
}

// La nova versió de printc admet dos paràmetres: el color de lletra i el color de fons
// Sabem que cada cel·la (Word ch) conté 1 byte pel codi ASCII i 1 byte pel codi color
// Bits 0-7: codi ASCII
// Bits 8-11: codi color foreground (color de lletra)
// Bits 12-15: codi color background (fons de lletra)
void printc_color(char c, Byte fg, Byte bg)
{
  bochs_out(c);
  if (c == '\n')
  {
    x = 0;
    y = (y + 1) % NUM_ROWS;
  }
  else
  {
    Word ch = (Word)(c & 0x00FF) | ((bg << 4 | fg) << 8);
    Word *screen = (Word *)0xb8000;
    screen[(y * NUM_COLUMNS + x)] = ch;
    if (++x >= NUM_COLUMNS)
    {
      x = 0;
      y = (y + 1) % NUM_ROWS;
    }
  }
}

void printc_xy(Byte mx, Byte my, char c, Byte fg, Byte bg)
{
  Byte cx, cy;
  cx = x;
  cy = y;
  x = mx;
  y = my;
  printc_color(c, fg, bg);
  x = cx;
  y = cy;
}

// Ara printk és un wrapper de printk_color amb colors per defecte
void printk(char *string)
{
  printk_color(string, WHITE, BLACK);
}

// La nova versió de printk imprimeix una string de chars segons fg i bg 
// sempre s'imprimiran 80 bytes, per omplir tota la línia de la pantalla
// és a dir, cada printk es farà en una línia diferent a menys que size>80
void printk_color(char *string, Byte fg, Byte bg)
{
  // escriu la cadena de caràcters desitjada
  for (int i = 0; string[i]; i++) 
    printc_color(string[i], fg, bg);
}

void print_hexa(unsigned int v)
{
  char hexa[] = "0123456789ABCDEF";

  printc_xy(45, 23, hexa[(v >> 28) & 0xF], WHITE, BLACK); // Dígit 8
  printc_xy(46, 23, hexa[(v >> 24) & 0xF], WHITE, BLACK); // Dígit 7
  printc_xy(47, 23, hexa[(v >> 20) & 0xF], WHITE, BLACK); // Dígit 6
  printc_xy(48, 23, hexa[(v >> 16) & 0xF], WHITE, BLACK); // Dígit 5
  printc_xy(49, 23, hexa[(v >> 12) & 0xF], WHITE, BLACK); // Dígit 4
  printc_xy(50, 23, hexa[(v >> 8) & 0xF],  WHITE, BLACK); // Dígit 3
  printc_xy(51, 23, hexa[(v >> 4) & 0xF],  WHITE, BLACK); // Dígit 2
  printc_xy(52, 23, hexa[v & 0xF], WHITE, BLACK); // Dígit 1
}

// Borra la info innecessària i crea una interfície de benvinguda
void welcome_screen(Byte fgcolor, Byte bgcolor)
{
  clear_screen(bgcolor);
  // missatges 
  printk_color("\n", fgcolor, bgcolor);
  printk_color(" Welcome to ZeOS! - Pau Segui and Joan Pere Ollers\n", fgcolor, bgcolor);
  printk_color("\n", fgcolor, bgcolor);
  printk_color(" 0x10-0x28    0x10-0x28 [25 pages] Reserved for Kernel memory\n", fgcolor, bgcolor);
  printk_color(" 0x90-0x90    0x90-0x90 [01 pages] Reserved for GDT\n", fgcolor, bgcolor);
  printk_color(" 0xB8-0xB8    0xB8-0xB8 [01 pages] Reserved for Screen buffer\n", fgcolor, bgcolor);
  printk_color(" 0x00-0x13    0x34-0x47 [20 pages] Reserved for User memory\n", fgcolor, bgcolor);
  printk_color(" 0x14-0x1B    0x2C-0x33 [08 pages] Reserved for User memory\n", fgcolor, bgcolor);  
  printk_color("\n", fgcolor, bgcolor);
  printk_color("\n", fgcolor, bgcolor);  
  printk_color(" Professor Baka-Baka loaded the kernel successfully...\n", fgcolor, bgcolor);
  printk_color(" Entering user mode...\n", fgcolor, bgcolor);
  printk_color(" -----------------------------------------------------------------\n", fgcolor, bgcolor);  
}

void clear_screen(Byte color)
{
  // netejar el fons de pantalla (pintar-ho tot amb espais buits)
  for (int i = 0; i < 25*80; ++i)
    printc_color(' ', color, color);
}
