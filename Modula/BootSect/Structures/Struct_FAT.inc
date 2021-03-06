; Bombela - 25/01/2004 - 03/02/2004
; Structures et constantes FAT

; ------------------------------------------------------
; Constante FAT_FILE_ATTR
; STRUC FAT_ENTRY
; ------------------------------------------------------

; ++++++++++++++++++++++++++++++++++++++++++++++++++++++
FAT_ENTRY_ATTR_READONLY equ 00000001b
FAT_ENTRY_ATTR_HIDEN    equ 00000010b
FAT_ENTRY_ATTR_SYSTEM   equ 00000100b
FAT_ENTRY_ATTR_LABEL    equ 00001000b
FAT_ENTRY_ATTR_DIR      equ 00010000b
FAT_ENTRY_ATTR_ARCHIVE  equ 00100000b
; ******************************************************

; ++++++++++++++++++++++++++++++++++++++++++++++++++++++
STRUC FAT_ENTRY

; Cette structure permet d'acc�der au r�pertoire et fichier de la FAT.

.Name               resb    8 ; Nom du r�pertoire ou de fichier
.Ext                resb    3 ; Extention du nom (exe,txt...)
.Attr               resb    1 ; Attribut du fichier

;Partie valide seulement pour la FAT32
;-----------------------------------------
  .Reserved_NT         resb    1 ; R�serv� pour Windows NT
  .CreateTimeTenth     resb    1 ; Dixi�mes de seconde d'heures de cr�ation
  .CreateTime          resb    2 ; Heure de cr�ation
  .CreateDate          resb    2 ; Date de cr�ation
  .LastAccessDate      resb    2 ; Date du dernier acc�s
  .FirstCluster_hi     resb    2 ; Partie haute du premier cluster de l'entr�e
;-----------------------------------------

.TimeModif          resb    2 ; Heure de la derni�re modification
.DateModif          resb    2 ; Date de la desni�re modification
.FirstCluster       resb    2 ; Premier clusteur du fichier
.Size               resb    4 ; Taille du fichier
ENDSTRUC
; ******************************************************

; ++++++++++++++++++++++++++++++++++++++++++++++++++++++

; ******************************************************