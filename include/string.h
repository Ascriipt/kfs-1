#ifndef STRING_H
#define STRING_H

#include "types.h"

/* ==============================================================================
 * include/string.h - Fonctions standards de manipulation memoire et chaines
 * ==============================================================================
 */

size_t strlen(const char *s);
int    strcmp(const char *s1, const char *s2);
int    strncmp(const char *s1, const char *s2, size_t n);
char  *strcpy(char *dest, const char *src);
char  *strncpy(char *dest, const char *src, size_t n);

/*
Même avec les options -fno-builtin, le compilateur GCC émet parfois en interne des appels à memcpy
ou memset (par exemple lorsqu'on copie une structure C ou qu'on initialise un tableau à zéro).
Implémenter ces fonctions évite des erreurs d'édition de liens (undefined reference to memcpy).
*/
void  *memset(void *s, int c, size_t n);
void  *memcpy(void *dest, const void *src, size_t n);
void  *memmove(void *dest, const void *src, size_t n);
int    memcmp(const void *s1, const void *s2, size_t n);

#endif /* STRING_H */
