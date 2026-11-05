#ifndef VGA_H
#define VGA_H

#include "types.h"

/* ==============================================================================
 * include/vga.h - Pilote d'affichage texte VGA (80x25)
 * ==============================================================================
 * En mode texte standard 80 colonnes x 25 lignes, la carte video VGA mappe sa
 * memoire a l'adresse physique 0xB8000.
 *
 * Chaque case a l'ecran est representee par 2 octets consecutifs (un uint16_t) :
 * - Octet de poids faible (bits 0-7)  : Code ASCII du caractere (ex: 'A' = 0x41)
 * - Octet de poids fort   (bits 8-15) : Attribut de couleur
 *     * bits 8-11  : Couleur du texte (Foreground, 16 couleurs possibles)
 *     * bits 12-14 : Couleur du fond (Background, 8 couleurs)
 *     * bit 15     : Clignotement (Blink)
 */

#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_MEMORY  ((volatile uint16_t *)0xB8000)

/* Les 16 couleurs standard du controleur VGA */
typedef enum {
    VGA_COLOR_BLACK         = 0,
    VGA_COLOR_BLUE          = 1,
    VGA_COLOR_GREEN         = 2,
    VGA_COLOR_CYAN          = 3,
    VGA_COLOR_RED           = 4,
    VGA_COLOR_MAGENTA       = 5,
    VGA_COLOR_BROWN         = 6,
    VGA_COLOR_LIGHT_GREY    = 7,
    VGA_COLOR_DARK_GREY     = 8,
    VGA_COLOR_LIGHT_BLUE    = 9,
    VGA_COLOR_LIGHT_GREEN   = 10,
    VGA_COLOR_LIGHT_CYAN    = 11,
    VGA_COLOR_LIGHT_RED     = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN   = 14,
    VGA_COLOR_WHITE         = 15
} vga_color_t;

/* Combine couleur de premier plan et couleur d'arriere-plan en un octet d'attribut */
static inline uint8_t vga_entry_color(vga_color_t fg, vga_color_t bg)
{
    return fg | (bg << 4);
}

/* Assemble le caractere ASCII et son attribut de couleur dans un mot de 16 bits */
static inline uint16_t vga_entry(unsigned char ch, uint8_t color)
{
    return (uint16_t)ch | ((uint16_t)color << 8);
}

/* Nombre d'ecrans virtuels supportes */
#define VGA_NUM_TTYS 3

/* Fonctions du driver VGA */
void   vga_init(void);
void   vga_clear(void);
void   vga_set_color(vga_color_t fg, vga_color_t bg);
void   vga_set_cursor(size_t x, size_t y);
void   vga_scroll(void);
void   vga_putchar(char c);
void   vga_puts(const char *str);

/* Gestion des ecrans virtuels (Terminaux TTY) */
void   vga_switch_tty(size_t tty_index);
size_t vga_get_current_tty(void);

#endif /* VGA_H */
