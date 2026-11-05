    #ifndef IO_H
#define IO_H

#include "types.h"

/* ==============================================================================
 * include/io.h - Instructions d'E/S sur les ports x86 (Port I/O)
 * ==============================================================================
 * Contrairement aux acces memoire RAM, l'architecture x86 possede un espace
 * d'adressage separe pour communiquer avec les peripheriques (clavier, carte
 * graphique, timer...). Cet espace utilise des instructions CPU specifiques :
 * 'in' (lire un port) et 'out' (ecrire sur un port).
 */

/**
 * Ecrit un octet (8 bits) sur un port I/O specifique.
 */
static inline void outb(uint16_t port, uint8_t value)
{
    // volatile : dit au compilateur de ne pas déplacer ni optimiser cette instruction
    __asm__ __volatile__("outb %0, %1" : : "a"(value), "Nd"(port));
}

/**
 * Lit un octet (8 bits) depuis un port I/O specifique.
 */
static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/**
 * Petite pause I/O (attend un cycle de bus pour les peripheriques lents).
 * Ecrire sur le port 0x80 (utilise par la carte POST du BIOS) est le standard.
 */
static inline void io_wait(void)
{
    outb(0x80, 0);
}

#endif /* IO_H */
