/*
	Ficher include copié automatiquement.
	Seul le fichier original doit être modifié.
*/

/*
 *	Petit pilote floppy.
 *
 *	Bombela.
 *	29/09/2004
 *	19/11/2004
 */

char Floppy_Init();
char Floppy_Release();
char Floppy_Read(u32 LBA,void* Buf);
char Floppy_Write(u32 LBA,void* Buf);
