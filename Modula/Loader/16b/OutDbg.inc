; Bombela
; Created: 25/06/2004
; Updated: 02/07/2004

; Fonctions pour le debug.
; Permettent d'afficher du texte en RMode et en PMode
; avec scroll. La chaîne est en ASCIIZ et en PMode
; les caractères de commandes ne sont pas reconnus.

; Dans tous les cas, les fonctions de
; sortie de texte passent à la ligne.

; Les fonctions PMode sont relative à une constante "Base"
; qui contient l'adresse de base physique du début loader.

; Info: C'est du code 16 bits en PMode.

LineSize	equ	80
NumLine		equ	25

CursorPos		db 0

OutDbg:
; Passe à la ligne et sort une chaîne.
; Entrée:
;   DS:SI = Chaîne.	
; Sortie:
;      SI = SI + Taille de la chaîne.
	mov ah, 0x0E
	mov al, 10
	int 0x10
	mov al, 13
	int 0x10
	call OutS
ret

PM_ReadCursorPos:
; Fonction en PMode.
; Récupère et sauve la position du curseur d'écran BIOS.
push eax
	xor eax, eax
	mov al, [0x0451+eax]
	mov [Base+CursorPos], al
pop eax
ret

PM_SetCursorPos:
; Fonction en PMode.
; Remplace la position du curseur d'écran BIOS
; par la position actuelle.
push eax
push ebx
	mov al, [Base+CursorPos]
	xor ebx, ebx
	mov [0x0451+ebx], al
pop ebx
pop eax
ret

PM_OutDbg:
; Fonction en PMode.
; Passe à la ligne et sort une chaîne simple.
; Entrée:
;   DS:ESI = Chaîne.	
pushfd
pushad
	mov edi, 0x000B8000
	movzx bx, byte [Base+CursorPos]
	
	cmp bx, NumLine-1
	jb .noscroll
	
		; Scrolling.
		
		xor bx, bx
		mov cx, ((NumLine-1)*(LineSize*2)) / 4
		
		.movedword
			mov eax, [edi+ebx+LineSize*2]
			mov [edi+ebx], eax
			add bx, 4
		loop .movedword
		
		; Free last line.
		
		mov bx, (NumLine-1)*(LineSize*2)
		mov cx, LineSize
		
		.putspace
			mov byte [edi+ebx], ' '
			inc bx
			inc bx
		loop .putspace
		
		mov bx, NumLine-1
		jmp .endscroll
	
.noscroll
	inc byte [Base+CursorPos]
	inc bx
.endscroll
	
	mov ax, LineSize*2
	mul bx
	movzx ebx, ax

	cld

.putchar
	lodsb
	test al, al
	jz .end
		cmp al, 10
		je .NewLine
			
			mov ah, 7
			mov [edi+ebx], ax
			inc bx
			inc bx
		
		.NewLine
		
		; Position du curseur video.
		push bx
		
		shr bx, 1
		
		CrtPort				equ	0x03D4
		RegCursorLow		equ 0x0F
		RegCursorHigh		equ 0x0E
		
		mov dx, CrtPort
		mov al, RegCursorLow
		mov ah, bl
		out dx, ax
		
		mov dx, CrtPort
		mov al, RegCursorHigh
		mov ah, bh
		out dx, ax
		
		pop bx
	jmp .putchar
.end
popad
popfd
ret