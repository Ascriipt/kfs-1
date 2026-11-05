#include "vga.h"
#include "io.h"
#include "string.h"

/* ==============================================================================
 * kernel/vga.c - Driver VGA Texte avec support Multi-Terminaux (TTYs)
 * ==============================================================================
 */

#define VGA_CRTC_INDEX_PORT 0x3D4
#define VGA_CRTC_DATA_PORT  0x3D5
#define VGA_CRTC_CURSOR_HI  0x0E
#define VGA_CRTC_CURSOR_LO  0x0F

typedef struct {
    uint16_t buffer[VGA_HEIGHT * VGA_WIDTH];
    size_t   row;
    size_t   column;
    uint8_t  color;
} tty_screen_t;

static tty_screen_t ttys[VGA_NUM_TTYS];
static size_t current_tty = 0;

void vga_set_cursor(size_t x, size_t y)
{
    uint16_t pos = (uint16_t)(y * VGA_WIDTH + x);

    outb(VGA_CRTC_INDEX_PORT, VGA_CRTC_CURSOR_HI);
    outb(VGA_CRTC_DATA_PORT, (uint8_t)((pos >> 8) & 0xFF));

    outb(VGA_CRTC_INDEX_PORT, VGA_CRTC_CURSOR_LO);
    outb(VGA_CRTC_DATA_PORT, (uint8_t)(pos & 0xFF));
}

void vga_scroll(void)
{
    tty_screen_t *tty = &ttys[current_tty];

    /* 1. Deplace chaque ligne vers le haut */
    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            tty->buffer[(y - 1) * VGA_WIDTH + x] = tty->buffer[y * VGA_WIDTH + x];
        }
    }

    /* 2. Efface la derniere ligne */
    uint16_t blank = vga_entry(' ', tty->color);
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        tty->buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = blank;
    }

    /* 3. Met a jour la memoire video physique */
    memcpy((void *)VGA_MEMORY, tty->buffer, sizeof(tty->buffer));
}

void vga_set_color(vga_color_t fg, vga_color_t bg)
{
    ttys[current_tty].color = vga_entry_color(fg, bg);
}

void vga_clear(void)
{
    tty_screen_t *tty = &ttys[current_tty];
    uint16_t blank = vga_entry(' ', tty->color);

    for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        tty->buffer[i] = blank;
    }

    tty->row = 0;
    tty->column = 0;

    memcpy((void *)VGA_MEMORY, tty->buffer, sizeof(tty->buffer));
    vga_set_cursor(tty->column, tty->row);
}

void vga_init(void)
{
    uint8_t default_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    uint16_t blank = vga_entry(' ', default_color);

    for (size_t t = 0; t < VGA_NUM_TTYS; t++) {
        ttys[t].row = 0;
        ttys[t].column = 0;
        ttys[t].color = default_color;
        for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
            ttys[t].buffer[i] = blank;
        }
    }

    current_tty = 0;
    vga_clear();
}

void vga_putchar(char c)
{
    tty_screen_t *tty = &ttys[current_tty];

    if (c == '\n') {
        tty->column = 0;
        tty->row++;
    } else if (c == '\r') {
        tty->column = 0;
    } else if (c == '\t') {
        tty->column = (tty->column + 8) & ~(8 - 1);
        if (tty->column >= VGA_WIDTH) {
            tty->column = 0;
            tty->row++;
        }
    } else if (c == '\b') {
        if (tty->column > 0) {
            tty->column--;
            size_t idx = tty->row * VGA_WIDTH + tty->column;
            tty->buffer[idx] = vga_entry(' ', tty->color);
            VGA_MEMORY[idx] = tty->buffer[idx];
        }
    } else {
        size_t idx = tty->row * VGA_WIDTH + tty->column;
        tty->buffer[idx] = vga_entry((unsigned char)c, tty->color);
        VGA_MEMORY[idx] = tty->buffer[idx];

        tty->column++;
        if (tty->column >= VGA_WIDTH) {
            tty->column = 0;
            tty->row++;
        }
    }

    if (tty->row >= VGA_HEIGHT) {
        vga_scroll();
        tty->row = VGA_HEIGHT - 1;
    }

    vga_set_cursor(tty->column, tty->row);
}

void vga_puts(const char *str)
{
    if (!str) return;
    for (size_t i = 0; str[i] != '\0'; i++) {
        vga_putchar(str[i]);
    }
}

size_t vga_get_current_tty(void)
{
    return current_tty;
}

void vga_switch_tty(size_t tty_index)
{
    if (tty_index >= VGA_NUM_TTYS || tty_index == current_tty) {
        return;
    }

    current_tty = tty_index;
    tty_screen_t *tty = &ttys[current_tty];

    /* Recopie le buffer du terminal actif dans la memoire video physique 0xB8000 */
    memcpy((void *)VGA_MEMORY, tty->buffer, sizeof(tty->buffer));

    /* Met a jour le curseur physique a la position du terminal actif */
    vga_set_cursor(tty->column, tty->row);
}
