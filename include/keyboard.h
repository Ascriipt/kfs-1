#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"

/* ==============================================================================
 * include/keyboard.h - Pilote du clavier PS/2 (Controleur Intel 8042)
 * ==============================================================================
 */

/* Ports I/O du controleur 8042 */
#define KEYBOARD_DATA_PORT   0x60
#define KEYBOARD_STATUS_PORT 0x64

/* Scancodes speciaux (Set 1 Make codes) */
#define SCANCODE_LSHIFT_DOWN 0x2A
#define SCANCODE_RSHIFT_DOWN 0x36
#define SCANCODE_LSHIFT_UP   0xAA
#define SCANCODE_RSHIFT_UP   0xB6
#define SCANCODE_CAPSLOCK    0x3A

#define SCANCODE_F1          0x3B
#define SCANCODE_F2          0x3C
#define SCANCODE_F3          0x3D
#define SCANCODE_BACKSPACE   0x0E
#define SCANCODE_ENTER       0x1C

/* Fonctions du driver clavier */
void    keyboard_init(void);
bool    keyboard_has_key(void);
uint8_t keyboard_get_scancode(void);
char    keyboard_scancode_to_char(uint8_t scancode);

#endif /* KEYBOARD_H */
