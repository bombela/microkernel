; Bombela - 25/01/2004 - 25/01/2004
; Structures relative au MBR (Master boot record)

; ------------------------------------------------------
; STRUC PARTITION_ITEM
; ------------------------------------------------------



; ++++++++++++++++++++++++++++++++++++++++++++++++++++++
; STRUC PARTITION_ITEM

; La table de partion comporte 4 fois cette structure.
; Elle contient les infos pour accéder au différentes
; partition d'un disque dur (4 max). La première se 
; trouve à l'offset 1BEh dans le MBR suivie des 3 autres.

STRUC PARTITION_ITEM
.state      resb    1 ; 80h = bootable, 0 = statique.
.head       resb    1 ; Tête
.sectcyl    resb    2 ; Secteur-Cylindre  
.type       resb    1 ; 04h = Fat16 bits, 11d = Fat32bits
.headf      resb    1 ; Tête de fin
.sectorf    resb    1 ; Secteur de fin  
.cyclef     resb    1 ; Cylindre de fin
.posi       resb    4 ; Secteur de la partion par rapport au mbr
.length     resb    4 ; Longeur de la partition en secteur   
ENDSTRUC
; ******************************************************