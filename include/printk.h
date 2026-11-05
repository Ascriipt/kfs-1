#ifndef PRINTK_H
#define PRINTK_H

#include "types.h"

/* ==============================================================================
 * include/printk.h - Fonctions d'affichage formatees pour le noyau (libk)
 * ==============================================================================
 * En freestanding (-ffreestanding / -nostdlib), nous utilisons les fonctions
 * intrinseques du compilateur GCC (__builtin_va_*) pour manipuler la liste des
 * arguments variables de maniere sure et portable sur architecture x86.
 */

typedef __builtin_va_list va_list;

#define va_start(v, l)  __builtin_va_start(v, l)
#define va_end(v)       __builtin_va_end(v)
#define va_arg(v, l)    __builtin_va_arg(v, l)
#define va_copy(d, s)   __builtin_va_copy(d, s)

int printk(const char *fmt, ...);
int vprintk(const char *fmt, va_list args);

#endif /* PRINTK_H */
