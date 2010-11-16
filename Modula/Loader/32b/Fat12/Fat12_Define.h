/*
 *	Pilote FAT12 très simple.
 *
 *	Bombela.
 *	20/11/2004
 *	12/12/2004
 */

#include <Types.h>

// Definitions.

#define Fat12_Eoc		0x0FFF // Valeur standard.
#define Fat12_ValidEoc	0x0FF8 // >= ValidEoc.
#define Fat12_Bad		0x0FF7 // Bobo...

#pragma pack (1) // Stop l'alignement.

typedef struct Fat12_Util
{
	u8	SecPerClus;
	u8	FatCnt;
	u16	FirstFatSec;
	u16	FatSz;
	u16	RootSec;
	u16	RootSecCnt;
	u16	FirstDataSec;
	u16	DirClus;
	u16	ClusterCnt;
} TFat12_Util;

typedef struct Fat12_BootSector
{
	u8	jmpboot[3];
	u8	OEMName[8];
	u16	u8sPerSec;
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

} TFat12_BootSector;

typedef struct Fat12_FileEntry
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

} TFat12_FileEntry;

#pragma pack () // Rétablit l'alignement par défaut.
