; Récupération de deux RDTSC.
; Intervalle entre les deux prise de 1ms.

; Bombela
; 01/02/2004
; 01/02/2004
; 13/02/2006 - Changement de Timer_SetCount en SetCount

extern Timer_Counter ; Le compteur de la procédure de timer.
extern Timer_On ; L'indicateur d'activation du timer.
extern SetCount ;¨Pour changer la vitesse du timer.


global CPU_Speed


; En entrée, un pinteur sur une structure
; de deux qword octets pour les RDTSC.

struc RDTSC2
.RDTSC_1	resq	1
.RDTSC_2	resq	1
endstruc


CPU_Speed:

%define PtrRDTSC2 ebp+8

enter 0, 0
pusha

  ; Préparation de la pause.
  mov dword [Timer_Counter], 1
  push dword 0
  call SetCount

  ; Récup du compteur dans EDX:EAX.
  RDTSC
  
  ; Sauv dans ECX:EBX
  mov ebx, eax
  mov ecx, edx
  
  ; Pause 55 ms.
  
  mov byte [Timer_On], 1
  .Pause
  	cmp byte [Timer_On] ,0
  jne .Pause  
  
  ; Récup du compteur dans EDX:EAX.
  RDTSC

  ; Copie des RDTSC dans la structure RDTSC2.
  mov edi, [PtrRDTSC2]
  mov [edi+RDTSC2.RDTSC_1+4],	ecx
  mov [edi+RDTSC2.RDTSC_1],	ebx
  mov [edi+RDTSC2.RDTSC_2+4],	edx
  mov [edi+RDTSC2.RDTSC_2],	eax

  push dword 0
  call SetCount
  add esp, 8 ; (2 Appel de SetCount).

popa
leave
ret