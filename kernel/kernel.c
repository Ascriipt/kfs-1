#include "types.h"
#include "vga.h"
#include "keyboard.h"
#include "string.h"
#include "printk.h"

/* ==============================================================================
 * kernel/kernel.c - Point d'entree C du noyau KFS_1
 * ==============================================================================
 */

#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002

static void init_tty1(void)
{
    vga_switch_tty(1);
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    printk("================================================================================\n");
    printk("                  TTY 1 - Terminal Interactif KFS_1                             \n");
    printk("================================================================================\n\n");
    printk("Tapez au clavier. Tout ce que vous tapez s'affiche ici en direct !\n");
    printk("Raccourcis : [F1] Ecran Boot '42' | [F2] TTY 1 Shell | [F3] TTY 2 Couleurs\n\n");
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    printk("kfs1@42:~$ ");
}

static void init_tty2(void)
{
    vga_switch_tty(2);
    vga_set_color(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
    printk("================================================================================\n");
    printk("                  TTY 2 - Palette VGA & Infos Noyau                             \n");
    printk("================================================================================\n\n");

    printk("Test des 16 couleurs du controleur VGA :\n\n");
    for (int i = 0; i < 16; i++) {
        vga_set_color((vga_color_t)i, VGA_COLOR_BLACK);
        printk(" [Couleur %2d] ", i);
        if ((i + 1) % 4 == 0) {
            printk("\n");
        }
    }

    printk("\n\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    printk("Architecture : x86 32-bit (i386 Protected Mode)\n");
    printk("Statut pile  : 16 Ko alloues dans .bss\n");
    printk("Pilotes      : Multiboot 1, VGA Text Mode (80x25), Clavier PS/2 (Set 1)\n");
}

void kernel_main(uint32_t magic, void *multiboot_info)
{
    /* 1. Initialisation des pilotes */
    vga_init();
    keyboard_init();

    /* 2. Verification du Magic Multiboot */
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        printk("[ERREUR] Magic Multiboot invalide : 0x%x (attendu : 0x%x)\n",
               magic, MULTIBOOT_BOOTLOADER_MAGIC);
        while (1) { }
    }

    /* 3. Configuration du TTY 0 (Boot et affichage '42') */
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    printk("================================================================================\n");
    printk("                   KFS_1 (Kernel From Scratch) - 42 Project                     \n");
    printk("================================================================================\n\n");

    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    printk("[OK] Boot Multiboot valide    : magic = 0x%x\n", magic);
    printk("[OK] Table Multiboot info     : %p\n", multiboot_info);
    printk("[OK] Pilote VGA & Libk charges\n");
    printk("[OK] Pilote Clavier PS/2 initialise\n");
    printk("[OK] 3 Terminaux virtuels configures\n\n");

    /* Affichage obligatoire du sujet : "42" */
    vga_set_color(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLUE);
    printk("  ********************  \n");
    printk("  *                  *  \n");
    printk("  *        42        *  \n");
    printk("  *                  *  \n");
    printk("  ********************  \n\n");

    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    printk("Raccourcis ecrans : [F1] Boot/'42' | [F2] TTY 1 Interactif | [F3] TTY 2 Infos\n");

    /* 4. Pre-initialisation des terminaux virtuels TTY 1 et TTY 2 */
    init_tty1();
    init_tty2();

    /* 5. On revient sur le terminal principal TTY 0 pour le demarrage */
    vga_switch_tty(0);

    /* 6. Boucle interactive principale (Scrutation / Polling clavier) */
    while (1) {
        if (keyboard_has_key()) {
            uint8_t scancode = keyboard_get_scancode();

            /* Raccourcis pour changer d'ecran virtuel */
            if (scancode == SCANCODE_F1) {
                vga_switch_tty(0);
            } else if (scancode == SCANCODE_F2) {
                vga_switch_tty(1);
            } else if (scancode == SCANCODE_F3) {
                vga_switch_tty(2);
            } else {
                /* Saisie normale au clavier */
                char c = keyboard_scancode_to_char(scancode);
                if (c) {
                    vga_putchar(c);
                    /* Si Entree sur TTY 1, réafficher le prompt */
                    if (c == '\n' && vga_get_current_tty() == 1) {
                        vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
                        printk("kfs1@42:~$ ");
                        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
                    }
                }
            }
        }
    }
}
