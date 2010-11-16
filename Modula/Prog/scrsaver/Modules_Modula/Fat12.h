/*
	Ficher include copié automatiquement.
	Seul le fichier original doit être modifié.
*/

/*
 *	Pilote FAT12 très simple.
 *
 *	Bombela.
 *	20/11/2004
 *	12/02/2005
 */

#include <types.h>

char Fat12_Init();

typedef struct
{
	u32 Pos;		// R/W

	// ------------- Read Only -------------
	u32 Size;
	u16 Cluster;
	u16 Entry_Cluster;
	u16 Entry_Offset;

} TFat12_FileInfo;

char Fat12_OpenFile(char *FileName,TFat12_FileInfo *FileInfo);
char Fat12_CloseFile(TFat12_FileInfo *FileInfo);
char Fat12_ReadFile(TFat12_FileInfo *FileInfo,u32 Count,void *Buf);
char Fat12_WriteFile(TFat12_FileInfo *FileInfo,u32 Count,void *Buf);
char Fat12_CreateFile(char *FileName, TFat12_FileInfo *FileInfo);
char Fat12_CreateDir(char *DirName);
char Fat12_ChangeDir(char *DirName);
char Fat12_IsDir(char *DirName);
char Fat12_IsFile(char *FileName);
