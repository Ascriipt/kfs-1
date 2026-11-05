#ifndef TYPES_H
#define TYPES_H

/* ==============================================================================
 * include/types.h - Types primitifs freestanding pour KFS_1
 * ==============================================================================
 * Dans un environnement sans OS (bare-metal) et sans libc standard, nous ne
 * pouvons pas inclure <stdint.h> ou <stddef.h>. Nous definissons donc nous-memes
 * les types a taille garantie pour l'architecture x86 32 bits (i386).
 */

/* Entiers non signes a taille fixe */
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;

/* Entiers signes a taille fixe */
typedef signed char         int8_t;
typedef signed short        int16_t;
typedef signed int          int32_t;
typedef signed long long    int64_t;

/* Types pour les adresses et tailles memoire */
typedef unsigned int        size_t;
typedef unsigned int        uintptr_t;
typedef signed int          intptr_t;

/* Booleens */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
/* En C23, bool, true et false sont des mots-cles natifs */
#elif !defined(__cplusplus)
typedef enum {
    false = 0,
    true  = 1
} bool;
#endif

/* Constante pointeur nul */
#ifndef NULL
#define NULL ((void *)0)
#endif

#endif /* TYPES_H */
