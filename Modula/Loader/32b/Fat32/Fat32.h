/*
 *	Pilote FAT32 très simple.
 *
 *	Bombela.
 */

#include <Types.h>


char Fat32_Init(TIDE_Device* DeviceIDE,u32 BaseSector);

typedef struct
{
	u32 Pos;		// R/W

	// ------------- Read Only -------------
	u32 Size;
	u32 Cluster;
	u32 Entry_Cluster;
	u16 Entry_Offset;

} TFat32_FileInfo;

char Fat32_ChangeDir(char *DirName);
char Fat32_OpenFile(char *FileName,TFat32_FileInfo *FileInfo);
char Fat32_CloseFile(TFat32_FileInfo *FileInfo);
char Fat32_ReadFile(TFat32_FileInfo *FileInfo,u32 Count,void *Buf);
char Fat32_IsDir(char *DirName);
char Fat32_IsFile(char *FileName);

/*
char Fat32_WriteFile(TFat12_FileInfo *FileInfo,u32 Count,void *Buf);
char Fat32_CreateFile(char *FileName, TFat12_FileInfo *FileInfo);
char Fat32_CreateDir(char *DirName);
*/
