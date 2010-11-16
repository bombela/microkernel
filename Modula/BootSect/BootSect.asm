; Boot sector FAT32.

; >Bombela<
; Created:      07/03/2004
; Last updated: 21/11/2004

; Lance Modula sur une partition FAT32.
; Sp�cial disquette.

; 9 octets de libre.

%define Version '1.1'

%include "Structures/Struct_FAT32.inc"
%include "Structures/Struct_FAT.inc"

Datas       equ 512
FatInfo     equ 512+4
Sector	    equ 512+4+512	
Cluster     equ 512+4+512+512

Loader      equ 0x08C0
 
use16
org 0x0

jmp Start

db '-MODULA-'
dw 512
db 1
dw 1
db 2
dw 224
dw 2880
db 0x0F0
dw 9
dw 18
dw 2
dd 0
dd 0
db 0
db 0

Start:

; Initialisation des segments en 0x07C0
	mov ax, 0x07C0
	mov ds, ax
	mov es, ax

	mov ax, 0x8000
	mov ss, ax
	mov sp, 0xFFFF

; LBA sur 28 bits = disque dur maximal de               128 Go
; LBA sur 32 bits = disque dur maximal de 	          2 048 Go
; LBA sur 48 bits = disque dur maximal de       134 217 728 Go
; LBA sur 64 bits = disque dur maximal de 8 796 093 022 208 Go

; Message d'intro
 	mov si, str_intro
  
; Affiche une cha�ne.
; Si 0xFF, � la fin, continue.
; Si 0x00, � la fin, termin�.
Print:
	lodsb
	cmp al, 0xFF
	je short Continue
	test al, al
	jz $
		mov ah, 0Eh
		mov bl, 14
		int 10h
	jmp short Print
Continue:

; Lit le BootSector.
	xor eax, eax
	mov bx, FatInfo
	mov cx, 1
	call ReadSector

; Calcul le d�but de la zone de donn�e.
; R�sultat dans le dword [Datas].
	movzx eax, byte [FatInfo+FAT32_INFO.NumberOfFat]
	mul dword [FatInfo+FAT32_INFO.FatSizeBig]
	movzx ebx, word [FatInfo+FAT32_INFO.ReservedSector]
	add eax, ebx
	mov [Datas], eax

; Cherche le dossier de PM3D.
	mov bp, Cluster
	mov eax, [FatInfo+FAT32_INFO.RootCluster]
	mov si, DirName
	call Chr_FindEntry_Clusteur
	
; Si pas trouv�e, erreur.
	mov si, str_dirnotfound
	test eax, eax
	jz  Print

; Cherche le loader de PM3D.
	mov bp, Cluster
	mov si, LoaderName	
	call Chr_FindEntry_Clusteur

; Si pas trouv�e, erreur.
	mov si, str_filenotfound
	test eax, eax
	jz  Print

; Chargement du loader.
	push word Loader
	pop es
	xor bp, bp
	mov  byte [ChrLoad], 1
	call Chr_FindEntry_Clusteur

	jmp dword Loader:0 ; Passe au loader.

; ----------------------------------------------------------------

; Functions

ReadSector:
; Lit un secteur sur le disque m�moris�.
; Si erreur, bootsector arr�t� avec msg.
; Entr�e:
;   EAX = Secteur � lire.
;    CX = Nombre de secteur � lire.
; ES:BX = Buffer o� tranf�rer les secteurs.
; Sortie:
;    BX Incr�ment� d'octets lut.
	pushad
	
	; Pr�pare un DAP dans la pile.
	o32 push byte 0 ; Bidouillage produisant un "push dword 0"
	                ; plus petit en taille d'instruction.
	add eax, [BootSect] ; Par rapport au BootSector...
	
	push eax
	push es
	push bx
	push cx
	push word 0x0010
	
	; La lecture.
	mov ah, 42h
	mov si, sp ; Adresse du DAP, qui est dans la pile.
	mov dl, [Disk]
	
	push ds
	push ss
	pop ds
	int 13h
	
	pop ds
	
	mov si, str_readerr ; Pr�pare la cha�ne d'erreur.
	jc Print ; Si erreur, affiche la cha�ne.
	add sp, 0x10 ; Vide le DAP de la pile.
	popad
	
	shl cx, 9 ; Convertion du nombres de secteurs en octets.
	add bx, cx ; Incr�mente le pointer du buffer.
	
ret

Chr_FindEntry_Clusteur:
; Fonction qui lit toutes une cha�ne de cluster
; et qui cherche une entr�e de fat dans chaques cluster.
; Si [ChrLoad] <> 0 alors les clusters sont charg�s
; � la suites et aucune entr�e n'est cherch�e.
; Le buffer utilis� est "Cluster".

; Entr�e:
;    EAX = Clusteur de d�part.
;(ES:)BP = Cluster de d�part.
;  Si ([ChrLoad] = 0) alors
;                 DS:SI = Pointeur sur l'entr�e � recherch�e.
; Sortie 
;  Si ([ChrLoad] = 0) alors
;                 EAX = Premier cluster de datas de l'entr�e.
;                 Si aucune entr�e trouver EAX = 0.

	push eax ; Sauvegarde du clusteur.

	; Convertion du cluster en secteurs.
	
	dec eax
	dec eax
	movzx ebx, byte [FatInfo+FAT32_INFO.SectorPerCluster]
	mul ebx
	add eax, [Datas]
	
	; Lecture du clusteur.
	xchg cx, bx ; Le nombre de secteurs par cluster dans cx.
	mov bx, bp
	call ReadSector ; Lecture du cluster.
	
	cmp byte [ChrLoad], 0
	jne .NotFind ; Pas de recherche d'entr�e.
		
		; Calcul le nombre d'entr�e dans un clusteur.
		movzx bx, byte [FatInfo+FAT32_INFO.SectorPerCluster]
		shl bx, 4 ; (16 Entr�es par secteurs).
		
		mov di, Cluster
		
		.FindEntry
						
			; Compare le nom.			
			push si
			push di
			mov cx, 11 ; Le nom complet fait 11 octets.
			rep cmpsb
			jne .NotFound
			
			; V�rifie l'octet d'attribut.
			lodsb
			cmp al, 0
			jl .MustNoAttr
				; Teste si attribut pr�sent.
				test al, [di]
				jz .NotFound
			jmp .EndAttr
			.MustNoAttr
				; Teste si attribut non pr�sent.
				neg al
				test al, [di]
				jnz .NotFound
			.EndAttr
			
			add sp, 8 ; Vide la pile de EAX,SI,DI.
			
			mov ax, [di+FAT_ENTRY.FirstCluster_hi-11]
			shl eax, 16
			mov ax, [di+FAT_ENTRY.FirstCluster-11]
				
			jmp .End ; Find de la proc�dure.
			
		.NotFound
		
			pop di
			pop si	
		
		add di, 32
		
		dec bx
		test bx, bx
		jnz .FindEntry
		jmp .FindEnd		
	
	.NotFind:
		mov bp, bx ; Incr�menter l'offset de chargement.
	.FindEnd:
	
	pop eax ; R�cup�re le cluster.
	
	; Calcul de la position du cluster
	; dans la table FAT.
	
	xor edx, edx
	xor ebx, ebx
	mov bl, 128
	div ebx ; eax = secteur FAT � charger et
	        ; dedans edx*4 = valeur du clusteur.
	
	movzx ebx, byte [FatInfo+FAT32_INFO.ReservedSector]
	add eax, ebx ; Ajoute d�but FAT.
	mov bx, Sector
	mov cx, 1
	
	push es
	push ds
	pop es
	
	call ReadSector
	
	pop es
	
	mov eax, [Sector+edx*4]
	
	cmp eax, FAT32_CLUSTER_EOF ; Si pas fin de cha�ne,
	jb Chr_FindEntry_Clusteur ; lire le cluster suivant.
	
	xor eax, eax
.End:
ret

; Datas.

; Le dernier octet est l'attribut.
; Si l'attribut ne doit pas �tre l�, mettre l'attribut en n�gatif.
DirName		db "MODULA     ", FAT_ENTRY_ATTR_DIR
LoaderName	db "LOADER  SYS", -FAT_ENTRY_ATTR_DIR

ChrLoad	  db 0 ; Lecture des clusters avec recherche d'un entr�e.

; Cha�nes de texte.
str_filenotfound db 10,13,"Loader.sys not found.",0
str_dirnotfound  db 10,13,"Modula not found.",0
str_readerr      db 10,13,"Read error.",0
str_intro	 db 10,13,"Modula BootSect-",Version,0xFF

times (512-($-$$))-7 db 0

; ---Position absolue---

Disk		db 80h
BootSect        dd 63

dw 0xAA55
