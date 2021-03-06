; Bombela  - 25/01/2004 - 25/01/2004
; Structures FAT12/FAT16

; ------------------------------------------------------
; STRUC FAT16_INFO
; ------------------------------------------------------

; ++++++++++++++++++++++++++++++++++++++++++++++++++++++
; STRUC FAT16_INFO

; Structure qui se trouve au d�but du BootSector d'une partition elle
; permet d'extraire les infos relatives aux syst�mes de fichiers FAT12/FAT16.

STRUC FAT16_INFO
.Jmp                resb  3 ; Saut vers le code du bootsector
.Name               resb  8 ; Nom et version du constructeur
.BytePerSector      resb  2 ; Nombre d'octets par secteurs
.SectorPerClust     resb  1 ; Nombre de secteurs par cluster
.SectorReserved     resb  2 ; Nombre de secteurs r�serv� (avant Fat #1)
.NbFat              resb  1 ; Nombre de Fat
.MaxEntryRoot       resb  2 ; Nombre d'entr�es max dans le roor (Fat 12)
.NbSector           resb  2 ; nombre de secteur sur un disque < 32 Mo
.Descriptor         resb  1 ; Descripteur de m�dia
.FatSize            resb  2 ; Taille de la fat en secteur (pas vu utilis�)
.SectorPerPiste     resb  2 ; Nombre de secteurs par piste
.NbHead             resb  2 ; Nombre de t�tes
.NbHideSector       resb  4 ; Nombre de scteurs cach�s
.NbSectorEx         resb  4 ; Nombre de secteur sur un disque > 32 Mo
.NumDisk            resb  1 ; Num�ro du disque (utilis� par Ms-Dos)
.Reserved           resb  1 ; Reserv�
.CheckEx            resb  1 ; Signature d'amorce �tendue = 29h
.IdVolume           resb  4 ; Num�ro d'identification binaire
.Label              resb  11; Nom du volume
.Type               resb  8 ; Type de syst�me de fichier
ENDSTRUC   
; ******************************************************
