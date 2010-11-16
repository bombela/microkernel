; Bombela
; Created: 25/06/2004
; Updated: 25/06/2004

OutS:
; Petite fonction qui sort une chaîne ASCIIZ à l'écran.
; Utilise le BIOS.
; Entrée:
;   DS:SI = Chaîne.
; Sortie:
;      SI = SI + taille de la chaîne.
pusha
mov ah, 0x0E

.putchar
	lodsb
	test al, al
	jz .end
		int 10h	
	jmp .putchar
.end

popa
ret