#include "keyboard.h"
#include "io.h"

/* ==============================================================================
 * kernel/keyboard.c - Implementation du driver clavier PS/2
 * ==============================================================================
 */

static bool shift_pressed = false;
static bool capslock_active = false;

/* Table de conversion Scancode Set 1 -> ASCII (sans Shift) */
static const char scancode_map_normal[128] = {
    0,   27,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,   '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0,   ' ', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

/* Table de conversion avec la touche Shift enfoncee */
static const char scancode_map_shift[128] = {
    0,   27,  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0,   '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0,   ' ', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

void keyboard_init(void)
{
    shift_pressed = false;
    capslock_active = false;

    /* Vide le buffer du controleur au cas ou des octets residuels traineraient */
    while (inb(KEYBOARD_STATUS_PORT) & 1)
        inb(KEYBOARD_DATA_PORT);
}

/**
 * Verifie si un octet est disponible dans le buffer du clavier (bit 0 du port 0x64).
 */
bool keyboard_has_key(void)
{
    return (inb(KEYBOARD_STATUS_PORT) & 1) != 0;
}

/**
 * Lit un scancode brut depuis le port 0x60.
 */
uint8_t keyboard_get_scancode(void)
{
    while (!keyboard_has_key())
        io_wait();
    return inb(KEYBOARD_DATA_PORT);
}

/**
 * Traite les touches modificatrices (Shift, Caps) et convertit en caractere ASCII.
 * Renvoie 0 si la touche n'est pas un caractere imprimable.
 */
char keyboard_scancode_to_char(uint8_t scancode)
{
    /* Gestion des touches relachees (Break codes : bit 7 a 1) */
    if (scancode & 0x80)
    {
        uint8_t released_code = scancode & 0x7F;
        if (released_code == SCANCODE_LSHIFT_DOWN || released_code == SCANCODE_RSHIFT_DOWN)
            shift_pressed = false;
        return 0;
    }

    /* Gestion des touches appuyees (Make codes) */
    if (scancode == SCANCODE_LSHIFT_DOWN || scancode == SCANCODE_RSHIFT_DOWN)
    {
        shift_pressed = true;
        return 0;
    }

    if (scancode == SCANCODE_CAPSLOCK)
    {
        capslock_active = !capslock_active;
        return 0;
    }

    if (scancode >= 128)
    {
        return 0;
    }

    char c = 0;

    if (shift_pressed) {
        c = scancode_map_shift[scancode];
    } else {
        c = scancode_map_normal[scancode];
    }

    /* Ajustement pour les lettres si CapsLock seul est actif */
    if (capslock_active && !shift_pressed) {
        if (c >= 'a' && c <= 'z') {
            c = c - 'a' + 'A';
        }
    } else if (capslock_active && shift_pressed) {
        if (c >= 'A' && c <= 'Z') {
            c = c - 'A' + 'a';
        }
    }

    return c;
}
