; ==============================================================================
; boot/boot.s - Point d'entree assembleur et header Multiboot
; ==============================================================================

; --- Constantes Multiboot 1 ---
MB_ALIGN_MODULES equ 1 << 0             ; Aligne les modules charges sur des pages de 4 Ko
MB_MEM_INFO      equ 1 << 1             ; Demande a GRUB de fournir une carte de la memoire
MB_FLAGS         equ MB_ALIGN_MODULES | MB_MEM_INFO
MB_MAGIC         equ 0x1BADB002         ; Magic number defini par la specification Multiboot 1
MB_CHECKSUM      equ -(MB_MAGIC + MB_FLAGS) ; La somme (magic + flags + checksum) doit valoir 0

; --- Section Multiboot ---
; Cette section DOIT se trouver dans les 8192 premiers octets de l'executable.
; Le script de linkage (linker.ld) la placera tout au debut du binaire.
section .multiboot
align 4
    dd MB_MAGIC                         ; 4 octets : Nombre magique
    dd MB_FLAGS                         ; 4 octets : Drapeaux
    dd MB_CHECKSUM                      ; 4 octets : Checksum de verification

; --- Section BSS : Allocation de la pile du noyau (Kernel Stack) ---
section .bss
align 16
stack_bottom:
    resb 16384                          ; Reserve 16 Ko pour la pile d'execution
stack_top:

; --- Section Texte : Code executable ---
section .text
global _start
extern kernel_main

_start:
    ; 1. Desactiver explicitement les interruptions materielles
    cli

    ; 2. Initialiser le pointeur de pile (ESP)
    ; Sur x86, la pile grandit vers les adresses basses (push = decrementation d'ESP).
    ; On place donc ESP tout en haut de l'espace alloue (stack_top).
    mov esp, stack_top

    ; 3. Reinitialiser le registre de drapeaux EFLAGS (bonne pratique)
    push 0
    popf

    ; 4. Passer les informations Multiboot a notre fonction C
    ; Convention d'appel C standard (cdecl 32 bits) : les arguments sont empiles de droite a gauche.
    ; kernel_main(uint32_t magic, multiboot_info_t* info)
    push ebx                            ; 2e argument : adresse de la structure multiboot_info
    push eax                            ; 1er argument : magic Multiboot (doit etre 0x2BADB002)

    ; 5. Appeler notre point d'entree C
    call kernel_main

    ; 6. Boucle infinie de secours si kernel_main retourne un jour
.hang:
    cli                                 ; Coupe les interruptions
    hlt                                 ; Met le processeur en pause jusqu'a la prochaine interruption
    jmp .hang                           ; Si une NMI survient, on reboucle
