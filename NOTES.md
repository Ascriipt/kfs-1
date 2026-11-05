# LINKER.ld

## Pourquoi commencer à 1 Mo (0x100000) ?
Sur l'architecture x86 (IBM PC), le premier mégaoctet de mémoire RAM (0x00000 à 0xFFFFF) est une zone      
historique très spéciale :
Plage mémoire                             │ Rôle matériel historique
───────────────────────────────────────────┼───────────────────────────────────────────────────────────────
0x00000 - 0x003FF                         │ Table des vecteurs d'interruptions du BIOS en Mode Réel (IVT)
0x00400 - 0x004FF                         │ Zone de données du BIOS (BDA)
0x07C00 - 0x07DFF                         │ Adresse où le BIOS charge le premier secteur de boot (MBR)
0xA0000 - 0xBFFFF                         │ Mémoire vidéo (dont 0xB8000 pour la mémoire texte VGA)
0xC0000 - 0xFFFFF                         │ BIOS de la carte mère et cartes d'extension

Si nous chargions notre noyau à l'adresse 0x0, nous écraserions ces structures matérielles et le BIOS !    

À partir de 0x100000 (1 Mo), on entre dans la mémoire étendue (Extended Memory), une zone de RAM contiguë  
et totalement libre. C'est l'adresse standard où les noyaux x86 32 bits (y compris Linux à ses débuts) se  
placent.


## Les blocs de section : .text : ALIGN(4K) { ... }
Regardons la structure d'une section :
.text : ALIGN(4K)
{
	*(.text)
}
Voici ce que chaque morceau signifie :
1. .text (à gauche des deux-points) :
C'est le nom de la section dans le fichier final.
2. : ALIGN(4K) :
C'est une directive d'alignement. Elle dit au linker : "Avant de commencer à écrire cette section,
assure-toi que l'adresse courante . est un multiple de 4 Ko (4096 octets)".
Si le curseur était à l'adresse 0x100020, le linker insère un petit rembourrage (padding) vide pour
sauter jusqu'à la prochaine frontière de 4 Ko (0x101000).
3. Les accolades { ... } :
Elles délimitent le contenu qui doit être injecté dans cette section de sortie.
4. L'expression *(.text) à l'intérieur :
C'est une règle de filtrage (avec des jokers / wildcards *) :
	• Le premier * signifie : n'importe quel fichier objet en entrée (boot.o, kernel.o, etc.).
	• Entre parenthèses (.text) signifie : la section nommée .text présente à l'intérieur de ce
	fichier.
- En clair, *(.text) ordonne au linker :
"Va chercher la section .text de CHAQUE fichier .o compilé, et concatène-les toutes ensemble, les unes
après les autres, à cet endroit précis."

## Pourquoi *(.rodata*) a deux étoiles ?

.rodata : ALIGN(4K)
{
	*(.rodata*)
}

Quand GCC compile des chaînes constantes (comme "42\n"), il crée parfois des sous-sections comme 
.rodata.str1.1 ou .rodata.cst4.
La deuxième étoile * permet d'attraper la section .rodata standard ET toutes ses sous-sections   
qui commencent par .rodata.
──────
## La section .bss et le mot-clé *(COMMON)

.bss : ALIGN(4K)
{
	*(COMMON)
	*(.bss)
}

• .bss est la section des variables globales non initialisées (par exemple notre pile de 16 Ko   
resb 16384). Ces données ne prennent pas de place sur le disque (elles sont juste marquées pour  
être mises à zéro par le système).
• *(COMMON) : en vieux standard C (et parfois selon les flags GCC), une variable globale non     
initialisée comme int compteur; était rangée dans une catégorie spéciale appelée "données        
communes" (common symbols). Mettre *(COMMON) garantit qu'elles soient bien regroupées dans le    
segment .bss.
──────
## La ligne finale : _kernel_end = .;

_kernel_end = .;

• Le point . vaut maintenant l'adresse mémoire exacte atteinte après avoir empilé toutes nos     
sections (.multiboot, .text, .rodata, .data, .bss).
• Nous créons une variable (un symbole exporté) appelée _kernel_end.
• Dans notre code C, nous pourrons écrire plus tard extern char _kernel_end; pour savoir         
exactement où s'arrête notre noyau en RAM (très utile quand on voudra coder un allocateur de     
mémoire kmalloc !).
──────
En résumé, ce fichier est un trieur automatique : il prend tous les morceaux éparpillés dans tes 
fichiers .o et les range proprement en mémoire à partir de 1 Mo, dans l'ordre exact dont GRUB et 
le CPU ont besoin.

#IO
Port I/O (PIO) : C'est un bus de communication séparé de la RAM, avec 65 536 "adresses de ports"    
numérotées de 0x0000 à 0xFFFF. Le C n'a aucun moyen natif d'y accéder, car il faut utiliser deux       
instructions spécifiques du processeur x86 :                                                           
	• in : lit un octet/mot depuis un port matériel vers un registre CPU.                              
	• out : envoie un octet/mot d'un registre CPU vers un port matériel.