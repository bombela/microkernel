/*
 *	Pilote FAT32 très simple.
 *
 *	Bombela.
 */

#include <Types.h>

// Definitions.

#define Fat32_Eoc		0xFFFFFFFF // Valeur standard.
#define Fat32_ValidEoc		0xFFFFFFF8 // >= ValidEoc.
#define Fat32_Bad		0xFFFFFFF7 // Bobo...

#pragma pack (1) // Stop l'alignement.

typedef struct Fat32_Util
{
	u32	SecPerClus;
	u16	ClusterSize;
	u32	FatCnt;
	u32	FirstFatSec;
	u32	FatSz;
	u32	RootCluster;
	u32	FirstDataSec;
	u32	DirClus;
	u32	ClusterCnt;
	
} TFat32_Util;

typedef struct Fat32_BootSector
{
	u8	jmpboot[3];
	u8	OEMName[8];
	u16	BytePerSec;
	u8	SecPerClus;
	u16	RsvdSecCnt;
	u8	FatCnt;
	u16	RootEntCnt;
	u16	TotSec;
	u8	Media;
	u16	FatSz;
	u16	SecPerTrk;
	u16	NumHeads;
	u32	HiddSec;
	u32	BigNumberOfSectors;
	u32	BigSectorPerFat;
	u16	ExtFlags;
	u16	VersionFS;
	u32	RootDirectoryStart;
	u16	InfoSector;
	u16	BackupBootSector;
	
} TFat32_BootSector;

typedef struct Fat32_FileEntry
{
	u8	Name[11];
	u8	Attr;
	u8	Rsrvd;
	u8	CrtTimeTenth; // 10 eme de seconde.
	u16	CrtTime;
	u16	CrtDate;
	u16	LstAccDate;
	u16	FstClusHi;
	u16	LstWrtTime;
	u16	LstWrtDate;
	u16	FstClusLo;
	u32	FileSize;

} TFat32_FileEntry;

#pragma pack () // Rétablit l'alignement par défaut.
