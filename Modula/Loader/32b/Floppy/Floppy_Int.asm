
; Bombela
; 03/10/2004
; 17/10/2004

global Floppy_Int

extern Floppy_IntCount
extern Pic_EndIrq

Floppy_Int:
	inc byte [Floppy_IntCount]

	push dword 13
	call Pic_EndIrq
	add esp, 4
iret