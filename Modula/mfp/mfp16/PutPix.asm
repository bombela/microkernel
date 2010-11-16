; Fonction qui permet d'allumer un pixel QBVR dans la mémoire vidéo en mode réel.
; La mémoire est donc aligné sur 4 octets...

; Bombela - supercar@tiscali.fr - 24/02/2004

; WORD: X et Y l'emplacement du point.
; WORD: Larg Largeur d'une ligne d'écran.
; 4 OCTETS: QBVR > Q = non utilisé, B = Bleu, V = Vert, R = Rouge

; Format d'appel: PASCAL.

global PutPixel

segment CODE

PutPixel:

%define QBVR bp+4
%define Larg bp+8
%define Y bp+10
%define X bp+12

enter 0,0
  pushad

mov ax, DATA
mov ds, ax

mov ax, [Y]
mov bx, [Larg]
mul bx

shl edx, 16
mov dx, ax

movzx eax, word [X]
add edx, eax

shl edx, 2

push edx
shr edx, 16

cmp dx, [SauvWin]
je .suit

mov [SauvWin], dx
mov ax, 4f05h
xor bx, bx
int 10h

.suit

pop ebx


mov ax, 0A000h
mov es, ax
mov ax, [QBVR]
mov [es:bx], ax
mov ax, [QBVR+2]
mov [es:bx+2], ax

  popad 
leave
ret 10

segment DATA

SauvWin dw 0