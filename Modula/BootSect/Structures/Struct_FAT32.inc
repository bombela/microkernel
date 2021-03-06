; Bombela  - 25/01/2004 - 15/02/2004
; Structures FAT32

; ------------------------------------------------------
; Constante FAT32
; STRUC FAT32_INFO
; ------------------------------------------------------

; ++++++++++++++++++++++++++++++++++++++++++++++++++++++
; Constantes dans la table FAT.

 FAT32_CLUSTER_FREE          equ 00000000h
 FAT32_CLUSTER_BAD           equ 0FFFFFF7h
 FAT32_CLUSTER_EOF           equ 0FFFFFF8h
; ******************************************************

; ++++++++++++++++++++++++++++++++++++++++++++++++++++++
; STRUC FAT32_INFO

; Structure qui se trouve au d�but du BootSector d'une partition elle
; permet d'extraire les infos relatives aux syst�mes de fichiers FAT32.

STRUC FAT32_INFO
.Jmp                resb  3 ; Saut vers le code du bootsector
.OemName            resb  8 ; Nom et version du constructeur

.BytePerSector      resb  2 ; Nombre d'octets par secteurs
.SectorPerCluster   resb  1 ; Nombre de secteurs par cluster
.ReservedSector     resb  2 ; Nombre de secteurs r�serv� (avant Table Fat #1)
.NumberOfFat        resb  1 ; Nombre de Fat
.RootEntries        resb  2 ; Nombre d'entr�es max dans le roor (Fat 12)
.TotalSector        resb  2 ; Nombre de secteur sur un disque
.Media              resb  1 ; Descripteur de m�dia
.FatSize            resb  2 ; Taille de la fat en secteur
.SectorPerTrack     resb  2 ; Nombre de secteurs par piste
.HeadPerCylinder    resb  2 ; Nombre de t�tes
.HideSector         resb  4 ; Nombre de scteurs cach�s
.NbSectorBig        resb  4 ; Nombre de secteur sur un disque
.FatSizeBig         resb  4 ; Taille de la fat pour un disque
.Flags              resb  2 ; ???
.Version            resb  2 ; ???
.RootCluster        resb  4 ; Num�ro du cluster du root
.InfoSector         resb  2 ; ???
.BootBackupStart    resb  2 ; ???
.Reserved           resb  12; Reserved

.DriveNumber        resb  1 ; Num�ro du disque (utilis� par Ms-Dos)
.Unused             resb  1 ; Reserv�
.ExtBootSignature   resb  1 ; Egale � 29h
.SerialNumber       resb  4 ; Num�ro d'identification binaire
.Label              resb  11; Nom du volume
.FileSystem         resb  8 ; Type de syst�me de fichier
ENDSTRUC   
; ******************************************************