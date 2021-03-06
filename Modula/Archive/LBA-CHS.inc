; Bombela  - 25/01/2004 - 05/02/2004
; Mode r�el
; Fonctions relative � la convertion LBA en CHS

; ## NON UTILISE ! ##

; ------------------------------------------------------
; Function LBA2CHS
; ------------------------------------------------------



; ++++++++++++++++++++++++++++++++++++++++++++++++++++++
LBA2CHS:

; Cette fonction fais la convertion LBA en CHS

; Entr�e:
; BP: Pointeur sur le boot sector en m�moire
; ES: Segment du pointeur
; ECX: LBA

; Sortie:
; DH: Num�ro de t�te
; CH: Num�ro de cynlindre
; CL: Num�ro de secteur

; Dans le prog principal inclure "Struct_FAT32.inc" 

push eax
push ebx

mov ax, cx
shr ecx, 16
mov dx, cx

div word [es:bp+FAT32_INFO.BPD+FAT32_BPD.SectorPerTrack]
inc dl
mov bl, dl
xor dx, dx
div word [es:bp+FAT32_INFO.BPD+FAT32_BPD.HeadPerCylinder]
shl dx, 8
mov cx, ax
xchg cl, ch
shl cl, 6
or cl, bl

pop ebx
pop eax

ret
; ******************************************************


; ++++++++++++++++++++++++++++++++++++++++++++++++++++++

; ******************************************************