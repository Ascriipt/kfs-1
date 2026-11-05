/*
 * ===============================================================================
 * Ce script indique au linker GNU (ld) comment organiser les differentes sections
 * de notre code et donnees dans le binaire final (kernel.bin) et a quelle adresse
 * physique le noyau doit etre charge en memoire RAM par GRUB.
 * ===============================================================================
*/

/* 1. Definir le point d'entree du noyau (symbole defini dans boot/boot.s) */
ENTRY(_start)

SECTIONS
{
    /* 2. On commence a l'adresse 1 Mo (0x100000).
     * Les adresses en dessous de 1 Mo sont reservees par l'architecture x86 pour
     * le BIOS, la BDA, les interruptions en mode reel et la memoire video (0xB8000).
     * Tout ce qui sera écrit après cette ligne sera logé à partir de cette adresse mémoire.
     */
    . = 1M;

    /* 3. Section Multiboot : DOIT etre placee en premier !
     * La specification Multiboot exige que le header soit dans les 8192 premiers
     * octets du fichier executable ELF.
     */
    .multiboot : ALIGN(4K)
    {
        *(.multiboot)
    }

    /* 4. Section .text : Le code machine executable (instructions ASM et C) */
    .text : ALIGN(4K)
    {
        *(.text)
    }

    /* 5. Section .rodata : Donnees en lecture seule (chaines de caracteres constantes, etc.) */
    .rodata : ALIGN(4K)
    {
        *(.rodata*)
    }

    /* 6. Section .data : Variables globales et statiques initialisees */
    .data : ALIGN(4K)
    {
        *(.data)
    }

    /* 7. Section .bss : Variables globales non initialisees et notre pile (stack) */
    .bss : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }

    /* Symboles de fin pour savoir ou se termine le noyau en memoire */
    _kernel_end = .;
}
