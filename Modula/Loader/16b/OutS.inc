; Bombela
; Created: 25/06/2004
; Updated: 25/06/2004

OutS:
; Petite fonction qui sort une cha�ne ASCIIZ � l'�cran.
; Utilise le BIOS.
; Entr�e:
;   DS:SI = Cha�ne.
; Sortie:
;      SI = SI + taille de la cha�ne.
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