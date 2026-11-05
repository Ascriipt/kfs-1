# ==============================================================================
# Makefile - KFS_1 (Kernel From Scratch)
# ==============================================================================

NAME        := kernel.bin
ISO         := kfs.iso

# Compilateurs et outils
CC          := gcc
AS          := nasm
LD          := ld
GRUB_RESCUE := $(shell which grub2-mkrescue 2>/dev/null || which grub-mkrescue 2>/dev/null)
QEMU        := qemu-system-i386

# Flags de compilation
# -m32 : Cible architecture x86 32-bit (i386)
# -ffreestanding : Pas d'environnement d'execution standard (pas de main standard ni libc)
# -fno-builtin : N'utilise pas les fonctions intrinseques/optimisees du compilateur (ex: memcpy)
# -fno-stack-protector : Desactive les canaris de pile qui necessitent __stack_chk_fail
# -nostdlib / -nodefaultlibs : Ne lie aucune bibliotheque standard du systeme hote
CFLAGS      := -m32 -std=gnu99 -ffreestanding -fno-builtin -fno-exceptions \
               -fno-stack-protector -nostdlib -nodefaultlibs \
               -Wall -Wextra -Werror -Iinclude

ASFLAGS     := -f elf32
LDFLAGS     := -m elf_i386 -T linker.ld

# Repertoires
SRCDIR      := kernel
BOOTDIR     := boot
OBJDIR      := obj
ISODIR      := iso

# Fichiers sources et objets
SRC_ASM     := $(wildcard $(BOOTDIR)/*.s)
SRC_C       := $(wildcard $(SRCDIR)/*.c)
OBJS        := $(patsubst $(BOOTDIR)/%.s, $(OBJDIR)/%.o, $(SRC_ASM)) \
               $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC_C))

# ==============================================================================
# Regles
# ==============================================================================

all: $(NAME) $(ISO)

$(NAME): $(OBJS) linker.ld
	@mkdir -p $(OBJDIR)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)
	@echo "[LD] $@ genere avec succes."

$(OBJDIR)/%.o: $(BOOTDIR)/%.s
	@mkdir -p $(OBJDIR)
	$(AS) $(ASFLAGS) $< -o $@
	@echo "[AS] $< -> $@"

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "[CC] $< -> $@"

# (--locales="" --fonts="" --themes="" :69) permet de retirer des polices, traduction etc que grub2-mkrescue 
# intègres par default. fait passser l'iso de 12Mo à ~3.5Mo.
$(ISO): $(NAME) $(ISODIR)/boot/grub/grub.cfg
	@if [ -z "$(GRUB_RESCUE)" ]; then \
		echo "Erreur: grub-mkrescue ou grub2-mkrescue introuvable !"; exit 1; \
	fi
	@mkdir -p $(ISODIR)/boot
	cp $(NAME) $(ISODIR)/boot/$(NAME)
	$(GRUB_RESCUE) --locales="" --fonts="" --themes="" -o $(ISO) $(ISODIR) 2>/dev/null 
	@echo "[ISO] $(ISO) creee avec succes."
	@echo -n "Taille de l'ISO: " && du -sh $(ISO)

run: $(ISO)
	$(QEMU) -cdrom $(ISO)

run-kernel: $(NAME)
	$(QEMU) -kernel $(NAME)

clean:
	rm -rf $(OBJDIR)
	rm -f $(ISODIR)/boot/$(NAME)
	@echo "[CLEAN] Objets supprimes."

fclean: clean
	rm -f $(NAME) $(ISO)
	@echo "[FCLEAN] Binaire et ISO supprimes."

re: fclean all

.PHONY: all iso run run-kernel clean fclean re
