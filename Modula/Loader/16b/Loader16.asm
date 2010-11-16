; Loader.
; Partie 16 bits.

; Bombela
; 25/06/2004
; 28/06/2004
; 06/02/2006 - Modification de la position de la pile. Ajout d'une constante.

%define DEBUG
%define Version '1.0'

;///////////////////////////////////////////////////////

[BITS 16]

LoadSeg	equ 0x08C0		; Segment de chargement.
Base 	equ LoadSeg*0x10	; L'adresse de chargement en 32 bits.
Pile	equ 0x00080000	; L'adresse de la pile en 32 bits.

; ------------------------------------------------------

global Loader16 ; Pour le linker, l'entry point.

Loader16:

	; Initialisation des segments.
	mov ax, LoadSeg
	mov ds, ax
	mov es, ax
	
	mov ax, ( ( Pile - 0x10000 ) / 0x10)  ; Le segment de la pile
	; -> on retire la taille de la pile en mode 16 bits à l'adresse de la pile en mode 32 bits.
	; -> ainsi, on à l'adresse de segment correcte c'est le pointeur de pile que feras la différence...
	mov ss, ax
	mov sp, 0xFFFF ; Le pointeur de pile en haut.

	mov si, str_intro
	call OutS

; Si debug, informe.

%ifdef DEBUG
	mov si, str_debugon
	call OutDbg
%endif

; Changement de mode texte.

%ifdef DEBUG
	;mov si, str_chngtext
	;call OutDbg
%endif

	;mov ax, 0x1111	; Changement de
	;mov bl, 0    	; résolution
	;int 10h       	; en 80x28

; Arrêt du moteur du lecteur disquette.

;%ifdef DEBUG
;	mov si, str_floppyoff
;	call OutDbg
;%endif

;	xor al, al
;	mov dx, 0x3F2
;	out dx, al

; Désactivation des interruptions.
	
%ifdef DEBUG
	mov si, str_cli
	call OutDbg
%endif

	cli

; Chargement de la gdt.

%ifdef DEBUG
	mov si, str_activegdt
	call OutDbg
%endif

	lgdt [GdtPtr]

; Activation de la ligne A20

%ifdef DEBUG
	mov si, str_activea20
	call OutDbg
%endif

        in      al, 92h
        or      al, 02h
        out     92h, al

; Passage en mode protégé.

%ifdef DEBUG
	mov si, str_pm
	call OutDbg
%endif

	mov eax, cr0
	or ax, 1
	mov cr0, eax

; Retablissement du selecteur de CS.

	jmp .CS_OK
.CS_OK

; Mise en place des selecteurs de DS, ES, FS, GS et SS.

	mov ax, 0x10 ; Le selecteur pointe sur le descripteur DATA.
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

; Le pointeur de pile.
	mov esp, Pile

%ifdef DEBUG
; Confirme le passage en PMode.
	call PM_ReadCursorPos ; Récup la position du curseur BIOS.

	mov si, Base+str_pmok
	call PM_OutDbg

	mov si, Base+str_jmp
	call PM_OutDbg

	; Met à jours le curseur BIOS.
	; Permet de ne pas décaler le driver d'affichage
	; du loader 32 bits.
	call PM_SetCursorPos
%endif

	; Passe le selecteur de CS en mode 32 bits
	; en spcécifiant un selecteur
	; (8 Indique le premier descripteur, celui du code)
	; et saute du même coup au code 32 bits.

	jmp dword 8:Base+Loader32

; ------------------------------------------------------

; Functions.

	%include "OutS.inc"

%ifdef DEBUG	
	%include "OutDbg.inc"
%endif
	
; Datas.

Gdt:
	dd 0, 0 ; Premier descripteur nul.
	dd 0x0000FFFF, 0x00CF9B00 ; Celui de code de 4go.
	dd 0x0000FFFF, 0x00CF9300 ; Et de data de 4go.

GdtPtr:
	dw	     24 ; Taille de la GDT (3*8).
	dd Base+Gdt ; Adresse de la GDT.

str_intro db 10,13,"Modula Loader16-",Version,0

%ifdef DEBUG
str_retour		db 10,13,0
str_debugon	db " [Loader16] : -- Modula Loader16 compilation DEBUG --",0
str_chngtext	db " [Loader16] : Activation du mode texte 80x28.",0
str_floppyoff	db " [Loader16] : Arret du moteur du lecteur disquette.",0
str_cli		db " [Loader16] : Desactivation des interruptions.",0
str_activegdt	db " [Loader16] : Chargement de la GDT.",0
str_activea20	db " [Loader16] : Activation de la ligne A20.",0
str_pm		db " [Loader16] : Passage en mode protege et mise en place des selecteurs...",0
str_pmok		db " [Loader16] : Mode protege !",0
str_jmp		db " [Loader16] : Passage en mode 32 bits...",0
%endif

; -------------------------------------------------------

times 1024-($-$$) db 0

Loader32: ; Le départ du code 32 bits.