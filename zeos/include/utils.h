/*
 * utils.h - Conté les funcions per intercanviar informació 
 * entre l'espai de memòria de l'usuari i el kernel 
 */

#ifndef UTILS_H
#define UTILS_H

void copy_data(void *start, void *dest, int size);
int copy_from_user(void *start, void *dest, int size);
int copy_to_user(void *start, void *dest, int size);

#define VERIFY_READ 0
#define VERIFY_WRITE    1

int access_ok(int type, const void *addr, unsigned long size);

#endif
