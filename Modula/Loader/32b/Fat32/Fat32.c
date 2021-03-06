/*
 *	Pilote FAT32 tr�s simple.
 *
 *	Travaille dans le dossier Modula sur la partition Modula.
 *
 *	Bombela.
 *	18/02/2006
 *	18/02/2006 - ReadFile.
 *	28/02/2006 - Correction de nombreux probl�me de calculs dans
 *	la table FAT ainsi que quelque d�passement de valeur.
 * 
 */

#include <Debug_Start.h>
#ifdef DEBUG_FAT32
#define DEBUG_NAME "FAT32   "
#endif
#include <Debug_End.h>

#include <Types.h>
#include <Arg.h>
#include <OutText.h>
#include <Ide.h>
#include <String.h>

#include "Fat32.h"
#include "Fat32_Define.h"
// -----------------------------------------

#define MAX_SECTOR_FAT 16 //< Le nombre de secteur de FAT en cache.

// -----------------------------------------

static TFat32_Util Util;

// Pour les fonctions qui travaille sur la FAT.
static u8	BufFat[512];
static u32 BufFatSector = 0xFFFFFFFF;
//static boolean BufFatModif = false;

static u8	BufFatRead[512];
static u32 BufFatReadSector = 0xFFFFFFFF;

u8 SectorFatModif[MAX_SECTOR_FAT];

// Device IDE
static TIDE_Device IDE_Device;
static u32 IDE_BaseSector;

// -----------------------------------------------------------------------

// Petites fonctions pratiques.

// 0 ok , 1 error.
static volatile u8 Read(u32 Sector,u8 SectorCount,u8* Buf)
{
	if (IDE_Read(&IDE_Device,IDE_BaseSector+Sector,SectorCount,Buf)) return 1; else return 0;
}

static u32 ClustToSec(u32 Cluster)
{
#ifdef DEBUG_FAT32
	u32 Sector = ((Cluster-2) * Util.SecPerClus) + Util.FirstDataSec;
	OutDbg("ClustToSec : Cluster %u > Sector %u.",Cluster,Sector);
	return Sector;
#else
	return ((Cluster-2) * Util.SecPerClus) + Util.FirstDataSec;
#endif
}

// 0 ok , 1 error.
static volatile u8 ReadCluster(u32 Cluster,u8* Buf)
{
	OutDbg("ReadCluster -> %u.",Cluster);
	if (IDE_Read(&IDE_Device,IDE_BaseSector+ClustToSec(Cluster),Util.SecPerClus,Buf)) return 1; else return 0;
}

static void ClustToSecAndOffInFat(u32 Cluster, u32 *Sector, u16 *Offset)
{
#ifdef DEBUG_FAT32
	u32 SaveCluster = Cluster;
#endif
	*Sector = Util.FirstFatSec + (Cluster / 128);
	*Offset = (Cluster % 128) * 4;
	OutDbg("ClustToSecAndOffInFat : Cluster %u > Sector %u, Offset %u.",SaveCluster,*Sector,*Offset);
}

// Copie rapide de zone m�moire.
void CopyMem(int Size,void *BufOut,void *BufIn)
{
	int i, ii;
	
	ii = Size / 4; // Nombre de u32 � copier.
	
	u32 *BufOutDw = BufOut;
	u32 *BufInDw = BufIn;
	for (i = 0; i < ii; i++) *(BufOutDw++)  = *(BufInDw++);
	
	ii = Size % 4; // Nombre de u8 restant � copier.
	
	u8 *BufOutBy = (void*) BufOutDw;
	u8 *BufInBy =  (void*) BufInDw;
	for (i = 0; i < ii; i++) *(BufOutBy++)  = *(BufInBy++);
}

// R�cup�re la valeur d'un cluster dans la FAT.
// ClusterValue contiendra la valeur du cluster trouv�.
// Renvoie 0 si ok.
static u8 GetClustValueInFat(u32 Cluster,u32* ClusterValue)
{
	u32 Sector;
	u16 Offset;
	u8 *ptrBuf = BufFatRead;
#ifdef DEBUG_FAT32
	u32 SaveCluster = Cluster;
#endif

	OutDbg("GetClustValueInFat cluster %u.",Cluster);

	ClustToSecAndOffInFat(Cluster,&Sector,&Offset);

	// Si le secteur de FAT est dans le buffer d'�criture, on l'utilise, sinon...
	if (Sector == BufFatSector)	ptrBuf = BufFat;
		// ...il est peut �tre dans le buffer de lecture.
		else if (Sector == BufFatReadSector) ptrBuf = BufFatRead;
			// Dans le cas contraire, il faut le lire...
			else
			{
				if (Read(Sector,1,BufFatRead)) return 1;
				BufFatReadSector = Sector;
			}
	
	*ClusterValue = *((u32*) &ptrBuf[Offset]);

	OutDbg("GetClustValueInFat : Cluster %u > Value %h.",SaveCluster,*ClusterValue);
	
	return 0;
}
/*
// Stock dans le table des secteurs de FAT modifi�,
// le secteur de FAT pass� en param�tre.
// La valeur est relative par rapport au d�but de la table FAT.
// Si return 0, c'est OK, sinon, plus de place dans la table.
char StockSectorFatModif(u8 FatSector)
{
	u8 i;
		
	OutDbg("StockSectorFatModif : %u.",FatSector);
		
	// Recherche si le secteur se trouve d�j� dans la table...
	for (i=0; i < MAX_SECTOR_FAT; i++)
	{
		if (SectorFatModif[i] == 0xFF)
		{
			// ...non trouv�, ajout...
			SectorFatModif[i] = FatSector;
			OutDbg("Secteur %u ajout� � SectorFatModif[]");
			return 0;
		}
		if (SectorFatModif[i] == FatSector)
		{
			OutDbg("Secteur %u pr�sent dans SectorFatModif[].",FatSector);
			return 0;
		}
	}
	
	OutDbg("Bizarre... Plus de place dans la table SectorFatModif...");
	return 1;
}

// Cette fonction vide le buffer de cache de la table FAT.
// Si pas 0, erreur !
char FlushBufFat()
{
	if ((!BufFatModif) || (BufFatSector == 0xFFFF))
	{
		OutDbg("FlushBufFat... Pas besoin.");
		return 0;
	}
	OutDbg("FlushBufFat... Secteur %u.",BufFatSector);
	if (Floppy_Write(BufFatSector,BufFat)) return 2;
	StockSectorFatModif(BufFatSector - Util.FirstFatSec);
	if (BufFatReadSector == BufFatSector) BufFatReadSector = 0xFFFF;
	BufFatSector = 0xFFFF;
	BufFatModif = false;
	return 0;
}

// Duplication de la premi�re FAT (Secteurs modifi�s) sur les autres FAT.
char FatDuplicate()
{
	u16 i,ii;
	u8 Sector[512];
	
	if (FlushBufFat()) return 10;
	
	OutDbg("Aplication des modifications sur les tables FAT secondaires...");
	for (i=0; i < MAX_SECTOR_FAT; i++)
	{
		if (SectorFatModif[i] == 0xFF) break;
		// Lecture d'un secteur modifi�...
		if (Floppy_Read(Util.FirstFatSec + SectorFatModif[i],Sector)) return 1;
		// Et �criture de ce secteur sur les autres table FAT.
		for (ii=1; ii < Util.FatCnt; ii++)
		{
			OutDbg("Copie du secteur %u de la table FAT primaire sur la table FAT %u...",SectorFatModif[i],ii+1);
			if (Floppy_Write(Util.FirstFatSec + (Util.FatSz * ii) + SectorFatModif[i],Sector)) return 2;	
		}
	}
	
	for (i=0; i < MAX_SECTOR_FAT; i++) SectorFatModif[i] = 0xFF;
		
	return 0;
}

// Si autre que 0, erreur !
char SetClustValueInFat(u16 Cluster, u16 NewValue)
{
	u16 Sector;
	u16 Offset;

	OutDbg("SetClustValueInFat : Cluster %u > Value %h.",Cluster,NewValue);

	ClustToSecAndOffInFat(Cluster,&Sector,&Offset);

	// Si le secteur de FAT n'est pas dans le buffer...
	if (Sector != BufFatSector)
	{	
		// ...on �crit le buffer...
		if (BufFatModif)
		{			
			if (Floppy_Write(BufFatSector,BufFat)) return 2;
			StockSectorFatModif(BufFatSector - Util.FirstFatSec);
			if (BufFatReadSector == BufFatSector) BufFatReadSector = 0xFFFF;
		}
		
		// ...et on le remplace par le bon secteur.
		if (Floppy_Read(Sector,BufFat)) return 1;
		BufFatSector = Sector;
	}
	
	BufFatModif = true;
	
	if (Offset == 511)
	{
		// Le cluster est sur deux secteur, on modifie et �crit le buffer...
		BufFat[511] = (NewValue & 0x0FFF) >> 4;
		if (Floppy_Write(BufFatSector,BufFat)) return 2;
		if (BufFatReadSector == BufFatSector) BufFatReadSector = 0xFFFF;
		
		// ...et on le remplace par le secteur de FAT suivant.
		if (Floppy_Read(BufFatSector++,BufFat)) return 1;
		BufFat[0] = (BufFat[0] & 0x0F) + (NewValue << 4);
	} else
	{
		// Definition pratique...
		#define Value (*((u16*) &BufFat[Offset]))
		
		// Modification de la bonne partie...
		if (Cluster & 1)
		{
			// Met � 0 les 12 bits fort et ajoute la nouvelle valeur.
			Value = (Value & 0x000F) + (NewValue << 4);
		}
		else
		{
			// Met � 0 les 12 bits faibles ajoute la nouvelle valeur.
			Value = (Value & 0xF000) + (NewValue & 0x0FFF);
		}
	}

	return 0;
}
*/

// 0 : Trouv� et *FileEntry actualis�.
// 1 : Rien trouv�.
// 2 : Rien trouv� et plus de FileEntry valide apr�s.
// Le FileEntry pointe dans le Buf.
static char FindValidEntry(TFat32_FileEntry **FileEntry,u8 *Buf)
{
	OutDbg("---------> %h > %h > %h",(u32) Buf,(u32) *FileEntry,((u32) Buf)+Util.ClusterSize);
	for (; (u32) *FileEntry < ((u32) Buf)+Util.ClusterSize; (*FileEntry)++)
	{
		if ((*FileEntry)->Name[0] == 0x00)
		{
			OutDbg("FindValidEntry -> Rien trouv� et y a plus rien apr�s.");
			return 2;
		}
		else
		{
			if (((*FileEntry)->Name[0] != 0x05) || ((*FileEntry)->Name[0] != 0xE5))
			{
				OutDbg("FindValidEntry -> Une entr�e trouv�e !");
				return 0;
			}
		}
	}
	OutDbg("FindValidEntry -> Rien trouv�.");
	return 1;
}

// 0 : Trouv� et *FileEntry actualis�.
// 1 : Trouv� et plus que des entr�es vides apr�s.
// 2 : Rien trouv�.
// Le FileEntry pointe dans le Buf.
static char FindFreeEntry(TFat32_FileEntry **FileEntry,u8 *Buf)
{
	for (; (u32) *FileEntry < ((u32) Buf)+Util.ClusterSize; (*FileEntry)++)
	{
		if ((*FileEntry)->Name[0] == 0x00) return 1;
		else
		if (((*FileEntry)->Name[0] == 0x05) || ((*FileEntry)->Name[0] == 0xE5)) return 0;
	}
	return 2;
}

/*
Cherche � partir de DirClust et de CurrentEntry dans le buffer Buf le premier fichier valide.
Si la fin d'un dossier atteint, le cluster suivant est charg� (sous
r�serve que ce ne soit pas une fin de cha�ne) et les variables pass�es
en param�tres sont actualis�es.
De m�me, CurrentEntry est actualis� quand un fichier valide est trouv�.
Pour pouvoir commencer au d�but d'un cluster, CurrentEntry doit �tre NULL.
ClustInBuf indique quel cluster est charg� dans Buf.
Si DirClust ne correspond pas, le cluster DirClust sera charg� dans buf.
CurrentEntry est un pointer qui se trouve quelque par dans Buf.

Retour :
0 : Un fichier, dossier ou nom de volume valide trouv�.
1 : Rien trouv�.
2 : Rien trouv� et il ne sert � rien de continuer, il n'y a plus d'entr�es (Vide ou non) apr�s.

Retour en cas de recherche d'entr�e vide :
0 : Une entr�e vide trouv�e.
1 : Une entr�e vide trouv�e et plus que des vides ensuite.
2 : Aucune entr�e vide trouv�e et plus d'entr�e du tout apr�s.

Autres : Erreurs diverses.
FreeEntry � true indique qu'il faut chercher une entr�e de fichier vide.
*/
static char FindFileEntry(u32 *DirClust, TFat32_FileEntry **CurrentEntry,u8 *Buf,u32 *ClustInBuf,boolean FreeEntry)
{
	OutDbg("FindFileEntry !");
	
	while (*DirClust < Fat32_ValidEoc)
	{
		// Chargement du cluster si besoin.
		if (*DirClust != *ClustInBuf)
		{
			OutDbg("Chargement du cluster %u (Pr�c�dent : %u).",*DirClust,*ClustInBuf);
			
			if (ReadCluster(*DirClust,Buf)) return 3;
				
			*ClustInBuf = *DirClust;
		}
		
		// Si CurrentEntry = 0 > On l'init au d�but du buffer sinon, on l'incr�mente.
		OutDbg("CurrentEntry%s;",(!*CurrentEntry?" = Buf":"++"));
		if (!*CurrentEntry) *CurrentEntry = (TFat32_FileEntry*) Buf; else (*CurrentEntry)++;
		OutDbg("CurrentEntry = %h",(u32) *CurrentEntry);
		
		if (FreeEntry)
		{
			switch (FindFreeEntry(CurrentEntry,Buf))
			{
				case 0: return 0;
				case 1: return 1;
			}
		}
		else
		{
			switch (FindValidEntry(CurrentEntry,Buf))
			{
				case 0: return 0;
				case 2: return 2;
			}
		}
		
		OutDbg("Chargement du cluster suivant.");
		if (GetClustValueInFat(*DirClust,DirClust)) return 3;
		*CurrentEntry = 0;
	}
	return 2;
}

/* Cherche une entr�e selon ;
 les attributs souhait�s : Attr_Ok.
 les attributs non voulu : Attr_No.
 le nom : *EntryName.
Si l'entr�e est trouv� :
  *EntryValClust contient le cluster des donn�es de l'entr�e.
Si EntryValClust = 0. *EntryValClust est ignor�.
  *EntryFileEntry contient le FileEntry de l'entr�e.
  Si EntryFileEntry = 0. *EntryFileEntry est ignor�.
De m�me *FileInfo est remplit si FileInfo <> 0.
0 = Trouv�.
*/
static char FindEntry(char *EntryName, u8 Attr_Ok, u8 Attr_No,u32 *EntryValClust,TFat32_FileEntry *EntryFileEntry, TFat32_FileInfo *FileInfo)
{
	u32 Cluster = Util.DirClus;
	u32 CurrentClust = 0;
	TFat32_FileEntry *FileEntry = 0;
	u8 Buf[Util.ClusterSize];
	char Res;
	
	OutDbg("FindEntry %s, Attr_Ok %u, Attr_No %u.",EntryName,Attr_Ok,Attr_No);

	// Recherche de l'entr�e...
	
	while (!(Res = (FindFileEntry(&Cluster,&FileEntry,Buf,&CurrentClust,false))))
	{
	#ifdef DEBUG_FAT32
		char DbgFileName[13];
		int i;
		
		for (i=0; i < 8; i++) DbgFileName[i] = FileEntry->Name[i];
		DbgFileName[8] = '.';
		for (i=8; i < 11; i++) DbgFileName[i+1] = FileEntry->Name[i];
		DbgFileName[12] = 0;
	#endif
		if (Res == 2) OutDbg("FindEntry -> Sers � rien de continuer y a plus rien.");
		
		if ( (!(FileEntry->Attr & Attr_No)) && (FileEntry->Attr & Attr_Ok) && (Str_nzCmp(FileEntry->Name,EntryName,11)) )
		{
			OutDbgColor(9,"\t\"%s\".",DbgFileName);
			if (EntryValClust) *EntryValClust = (FileEntry->FstClusHi << 16) + FileEntry->FstClusLo;
			if (EntryFileEntry) *EntryFileEntry = *FileEntry;
			if (FileInfo)
			{
				FileInfo->Cluster = (FileEntry->FstClusHi << 16) + FileEntry->FstClusLo;
				FileInfo->Size = FileEntry->FileSize;
				FileInfo->Pos = 0;
				FileInfo->Entry_Cluster = Cluster;
				FileInfo->Entry_Offset = (u32) (FileEntry) - (u32) (Buf);	
			}
			OutDbg("FindEntry OK -> EntryClust %u, EntryOffset %u, EntryValClust %u.",Cluster,(u32) (FileEntry) - (u32) (Buf),(FileEntry->FstClusHi << 16) + FileEntry->FstClusLo);
			return 0;
		}
		OutDbg("\t\"%s\".",DbgFileName);
		OutDbg("Rejet : Test Attr_No = %u, Test Attr_Ok = %u, Test Str_nzCmp = %u",(FileEntry->Attr & Attr_No),(FileEntry->Attr & Attr_Ok),(Str_nzCmp(FileEntry->Name,EntryName,11)));
	}
	return 1;
}

char Fat32_Init(TIDE_Device* DeviceIDE,u32 BaseSector)
{
	u8 Buf[512];
	TFat32_BootSector* BootSec;
	int i;

	OutDbg("-- Pilote FAT32 tr�s simple. DEBUG --");
	IDE_Device = *DeviceIDE;
	IDE_BaseSector = BaseSector;
	
	for (i=0; i < MAX_SECTOR_FAT; i++) SectorFatModif[i] = 0xFF; // Pour bien commencer.

	OutDbg("Lecture du BootSector et calculs de base...");
	if (Read(0,1,Buf)) return 1;

	BootSec = (TFat32_BootSector*) Buf;

	Util.SecPerClus = BootSec->SecPerClus;
	Util.ClusterSize = Util.SecPerClus * 512;
	Util.FatCnt = BootSec->FatCnt;
	Util.FatSz = BootSec->BigSectorPerFat;
	Util.FirstFatSec = BootSec->RsvdSecCnt;
	Util.RootCluster = BootSec->RootDirectoryStart;
	Util.FirstDataSec = Util.FirstFatSec + (Util.FatCnt * Util.FatSz);
	Util.ClusterCnt = (BootSec->BigNumberOfSectors - (Util.FatSz * Util.FatCnt));

	OutDbg("Util.FirstDataSec %u, Util.RootCluster %u.",Util.FirstDataSec,Util.RootCluster);
	OutDbg("OEM : %s.",(char*) BootSec->OEMName);

	Util.DirClus = Util.RootCluster; // Hop la racine.
	
	return 0;
}

// = 0 : OK.
// <>0 : Err.
char Fat32_OpenFile(char *FileName,TFat32_FileInfo *FileInfo)
{
	OutDbg("OpenFile %s.",FileName);
	if (!FindEntry(FileName,0xFF,8+16,NULL,NULL,FileInfo)) return 0;
	return 1;
}

// = 0 : OK.
// <>0 : Err.
// -> 1 : Erreur de lecture.
// -> 2 : Curseur de lecture en fin de fichier et/ou count trop grand pour ce
//			qu'il peut �tre lut depuis le curseur.
char Fat32_ReadFile(TFat32_FileInfo *FileInfo,u32 Count,void *Buf)
{
	u32 NbStartCluster = FileInfo->Pos / (Util.ClusterSize);
	u32 PosInCluster = FileInfo->Pos % (Util.ClusterSize);
	u32 Cluster = FileInfo->Cluster;
	u32 i = 0,iii;
	u8 ClusterBuf[Util.ClusterSize];

	OutDbg("ReadFile... Pos(%u), Count(%u), Size(%u).",FileInfo->Pos,Count,FileInfo->Size);

	if ((FileInfo->Pos + Count) > FileInfo->Size) return 2;

	// Recherche du NbStartCluster...
	OutDbg("Cherche le premier cluster du fichier � lire...");
	Cluster = FileInfo->Cluster;
	for (; i < NbStartCluster; i++)
	{
		if (GetClustValueInFat(Cluster,&Cluster)) return 1;
	}
	OutDbg("Premier cluster � lire : %u.",Cluster);

	// Copie du premier cluster si il est pas entier...
	if ((PosInCluster > 0) || (Count < (Util.ClusterSize)))
	{
		u8 *ptrClusterBuf = ClusterBuf + PosInCluster;
		
		OutDbg("Premier cluster pas entier...");
		
		// Lecture du premier cluster...
		if (ReadCluster(Cluster,ClusterBuf)) return 1;
		
		//Nombre de u8 � copier.
		if (Count > (Util.ClusterSize))
			iii = (Util.ClusterSize)-PosInCluster;
		else
			iii = (FileInfo->Pos + Count) - PosInCluster;
		
		CopyMem(iii,Buf,ptrClusterBuf);
		Buf += iii;
		ptrClusterBuf += iii;
		
		Count -= iii;
		FileInfo->Pos += iii;
		
		if (!Count) return 0;
		// Pour les clusters suivant.
		if (GetClustValueInFat(Cluster,&Cluster)) return 1;
	}

	NbStartCluster = Count / (Util.ClusterSize); // Nombre de cluster entier � copier.
	PosInCluster = Count % (Util.ClusterSize);   // Nombre d'octets dans le dernier cluster.

	// Copie des cluster entiers si il y en a...
	for (i = 0; i < NbStartCluster; i++)
	{
		OutDbg("Copie du cluster entier %u sur %u.",i+1,NbStartCluster);
		if (ReadCluster(Cluster,Buf)) return 1;
		Buf += (Util.ClusterSize);
		FileInfo->Pos += (Util.ClusterSize);
		if (GetClustValueInFat(Cluster,&Cluster)) return 1;
	}
	
	// Copie du dernier cluster si il est pas entier...
	if (PosInCluster > 0)
	{
		u8 *ptrClusterBuf = ClusterBuf;
		
		OutDbg("Dernier cluster pas entier...");
		if (ReadCluster(Cluster,ClusterBuf)) return 1;
		CopyMem(PosInCluster,Buf,ptrClusterBuf);
		FileInfo->Pos += PosInCluster;
	}

	return 0;
}
/*
// Alloue des clusters...
// Renvoit 0 si OK.
// Si FirstCluster = 0, l'allocation commence n'importe o�
// et FirstCluster est actualis� avec le premier cluster de la cha�ne.
// Si FirstCluster contient l'adresse d'un cluster, une v�rification de l'existence
// possible d'une suite de cha�ne est v�rifi�e. Donc, une fin de cha�ne valide doit exister.
// Avantage, m�me si des clusters sont d�j� allou�, seul sont ajout� le nombre nessessaire. 
// ATTENTION ! Ne pr�pare pas et ne termine pas l'utilisation du buffer de la table FAT !
// ATTENTION ! N'iniatialise pas et ne lance pas la recopie des secteurs de FAT modifi�. 
char ClusterAlloc(u16 *FirstCluster, u16 ClusterAllocCount)
{
	u16 i, ii = 0, Cluster = 0;
	
	OutDbg("Allocation de %u Cluster(s) � partir du cluster %u.",ClusterAllocCount,*FirstCluster);

	// Cherche le dernier Cluster de la cha�ne pour �viter les mauvaises surprises...
	if (*FirstCluster > 1)
	{
		OutDbg("Cherche le dernier Cluster de la chaine...");
		i = *FirstCluster;
		while (i < Fat12_ValidEoc)
		{
			Cluster = i;
			if (GetClustValueInFat(Cluster,&i)) return 1;
			ii++;
		}
		OutDbg("Dernier Cluster de la cha�ne : %u.",Cluster);
	
		if (ii >= ClusterAllocCount)
		{
			OutDbgColor(11,"Il y a d�j� %u cluster allou� (%u demand�).",ii,ClusterAllocCount);
			return 0;
		}
		else
			if (ii)
			{
				ClusterAllocCount -= ii;
				OutDbgColor(11,"Il y a d�j� %u cluster(s) allou�. PLus que %u � allouer.",ii,ClusterAllocCount);
			}
	}
	
	ii = 1;
	
	OutDbg("Allocation des cluster...");
	
	// L'allocation des Cluster...
	for (i=2; i < Util.ClusterCnt; i++)
	{
		u16 TestClust;
		if (GetClustValueInFat(i,&TestClust)) return 1;
		if (TestClust == 0)
		{
			if (*FirstCluster < 2) *FirstCluster = i;
				else if (SetClustValueInFat(Cluster,i)) return 7;
			
			Cluster = i;
			ClusterAllocCount--;
			if (ClusterAllocCount == 0)
			{
				ii = 0; // Allocation termin�e.
				OutDbg("Allocation termin�e...");
				break;
			}	
		}	
	}
	
	OutDbg("Ecriture de la fin de cha�ne.");
	if (SetClustValueInFat(Cluster,Fat12_Eoc)) return 7; // La fin de l'allocation !
	
	if (ii)
	{
		OutDbg("Plus de place sur la disquette !");
		return 6;
	}
	
	return 0;
}


// Si renvoit 0 c'est OK.
char Fat12_WriteFile(TFat12_FileInfo *FileInfo,u32 Count,void *Buf)
{
	u16 NbStartCluster = (FileInfo->Pos / 512); // Premier cluster � modifier.
	u16 PosInCluster = (FileInfo->Pos % 512); // Et premier u8 � modifier.
	u16 Cluster = FileInfo->Cluster; // Le premier cluster du fichier.
	u32 NewSize = (FileInfo->Pos + Count);
	u16 ClusterAllocCount = ((NewSize + 511) / 512); // Nombre de clusters.
	u16 i, iii;
	u16 Lba;
	u8 Sector[512];

	// La taille du fichier apr�s l'�criture des donn�es.
	if (FileInfo->Size > NewSize) NewSize = FileInfo->Size;
	
	OutDbg("WriteFile... Pos(%u), Count(%u).",FileInfo->Pos,Count);
	OutDbg("Size %u, NewSize %u, ClusterAllocCount %u.",FileInfo->Size,NewSize,ClusterAllocCount);

	OutDbg("Cluster %u.",Cluster);
	ClusterAlloc(&Cluster,ClusterAllocCount);
	FileInfo->Cluster = Cluster; // Si Cluster �tait � z�ro, sa valeur � chang�e !

	// On cherche le num�ro du premier cluster � modifier...
	OutDbg("Cherche le premier cluster du fichier � modifier...");
	Cluster = FileInfo->Cluster;
	for (i=0; i < NbStartCluster; i++)
	{
		if (GetClustValueInFat(Cluster,&Cluster)) return 1;
	}
	OutDbg("Premier cluster � modifier : %u.",Cluster);

	// Ecriture du premier cluster si il est pas entier...
	if ((PosInCluster > 0) || (Count < 512))
	{
		u8 *ptrSector = Sector + PosInCluster;
		
		OutDbg("Premier cluster pas entier...");
		
		// Lecture du premier secteur...
		Lba = ClustToSec(Cluster);
		if (Floppy_Read(Lba,Sector)) return 1;

		//Nombre de u8s � �crire dans ce secteur.
		if (Count > 512)
			iii = 512-PosInCluster;
		else
			iii = NewSize - (NbStartCluster * 512) - PosInCluster;
		
		CopyMem(iii,ptrSector,Buf);
		ptrSector += iii;
		Buf += iii;
		
		// Ecriture du secteur modifi�...
		if (Floppy_Write(Lba,Sector)) return 1;
		
		Count -= iii;
		
		FileInfo->Pos += iii;
		
		if (Count)
		{
			// Pour les clusters suivant.
			if (GetClustValueInFat(Cluster,&Cluster)) return 1;
			OutDbg("Adresse du prochain cluster : %u.",Cluster);
		}
	}
	
	NbStartCluster = Count / 512; // Nombre de cluster entier � �crire.
	PosInCluster = Count % 512;   // Nombre d'octets dans le dernier cluster.
	
	// Copie des cluster entiers si il y en a...
	for (i = 0; i < NbStartCluster; i++)
	{
		OutDbg("Copie du cluster entier %u sur %u.",i+1,NbStartCluster);
		if (Floppy_Write(ClustToSec(Cluster),Buf)) return 1;
		Buf += 512;
		FileInfo->Pos += 512;
		if (GetClustValueInFat(Cluster,&Cluster)) return 1;
	}

	// Ecriture du dernier cluster si il est pas entier...
	if (PosInCluster > 0)
	{
		u8 *ptrSector = Sector;
		
		OutDbg("Dernier cluster pas entier...");

		// Dabord la lecture...
		Lba = ClustToSec(Cluster);
		if (Floppy_Read(Lba,Sector)) return 1;

		CopyMem(PosInCluster,ptrSector,Buf);
		ptrSector += PosInCluster;
		Buf += PosInCluster;

		// Ecriture du secteur modifi�.
		if (Floppy_Write(Lba,Sector)) return 1;

		FileInfo->Pos += PosInCluster;
	}

	OutDbg("Modification du FileEntry...");

	// Modification du FileEntry avec la nouvelle taille et cluster.
	Lba = ClustToSec(FileInfo->Entry_Cluster);
	if (Floppy_Read(Lba,Sector)) return 1;
	((TFat12_FileEntry*) (Sector + FileInfo->Entry_Offset))->FileSize = NewSize;
	((TFat12_FileEntry*) (Sector + FileInfo->Entry_Offset))->FstClusHi = 0;	
	((TFat12_FileEntry*) (Sector + FileInfo->Entry_Offset))->FstClusLo = FileInfo->Cluster;
	if (Floppy_Write(Lba,Sector)) return 2;

	FileInfo->Size = NewSize;// Mise � jours de la taille...

	FatDuplicate();	// Et on recopie tous les secteurs FAT modifi� sur les autres FAT.
	
	return 0;
}

*/
char Fat32_CloseFile(TFat32_FileInfo *FileInfo)
{
	OutDbg("CloseFile !");

	FileInfo->Cluster = 0;
	FileInfo->Pos = 0;
	FileInfo->Size = 0;
	FileInfo->Entry_Cluster = 0;
	FileInfo->Entry_Offset = 0;
	return 0;
}
/*
// Cr�e une entr�e dans le r�pertoire courant.
// FatDuplicate() non appel�.
// 0 = ok.
// Attention ! N'�crit pas sur la disquette le cluster contenant la nouvelle entr�e.
// EntrySector contient le num de secteur contenant la nouvelle entr�e.
char CreateEntry(char *EntryName, TFat12_FileInfo *FileInfo, u8 Attr, u8 *Buf,TFat12_FileEntry **FileEntry, u16 *EntrySector)
{
	u16 Cluster = Util.DirClus;
	u16 CurrentClust = 0;
	char Res;
	u8 i;

	OutDbg("CreateEntry '%s', attributs %b1.",EntryName,Attr);
	
	OutDbg("Verification de l'unit�e du nom choisi...");
	if (!FindEntry(EntryName,0xFF,8,NULL,NULL,NULL))
	{
		OutDbg("Le nom choisi (%s) existe d�j�...",EntryName);
		return 6;
	}

	OutDbg("Recherche d'un emplacement vide.");
	
	Cluster = Util.DirClus;
	*FileEntry = 0;
	
	// Recherche d'une place pour le nouveau file entry.
	if ((Res = (FindFileEntry(&Cluster,FileEntry,Buf,&CurrentClust,true))) == 2)
	{
		// Il Faut ajouter un cluster.
		OutDbg("Ajout d'un cluster...");
		if (IsRootCluster(Cluster))
		{
			OutDbg("Impossible d'ajouter un cluster � la racine.");
			return 5; // On peut pas ajouter des clusters root.
		}
		ClusterAlloc(&Cluster,1);
	} else if (Res > 2) return 4;
	
	OutDbg("Ecriture du FileEntry...");
	
	FileInfo->Pos = 0;
	FileInfo->Size = 0;
	FileInfo->Cluster = 0;
	FileInfo->Entry_Cluster = Cluster;
	FileInfo->Entry_Offset = (u16) (((u32) (*FileEntry)) - ((u32) Buf));
	
	(*FileEntry)->Attr = Attr;
	
	for(i=0; i < 11; i++) (*FileEntry)->Name[i] = EntryName[i];
	
	(*FileEntry)->FstClusHi = 0;
	(*FileEntry)->FstClusLo = 0;
	(*FileEntry)->FileSize = 0;
	(*FileEntry)->Rsrvd = 0;
	(*FileEntry)->CrtTimeTenth = 0;
	(*FileEntry)->CrtTime = 0;
	(*FileEntry)->CrtDate = 0;
	(*FileEntry)->LstAccDate = 0;
	(*FileEntry)->LstWrtTime = 0;
	(*FileEntry)->LstWrtDate = 0;
	
	// Si il n'y avait plus que des entr�es vides, il faut l'indiquer.
	if ((Res == 1) && (((u32) (*FileEntry)+1) < ((u32) Buf+512))) 
	{
		OutDbg("Il n'y a plus que des entr�e vide apr�s.");
		((*FileEntry)+1)->Name[0] = 0;
	}
		
	if (!IsRootCluster(Cluster)) Cluster = ClustToSec(Cluster);
	
	*EntrySector = Cluster;	
	
	return 0;
}

// 0 = ok
char Fat12_CreateFile(char *FileName, TFat12_FileInfo *FileInfo)
{
	char Buf[512];
	TFat12_FileEntry *FileEntry;
	u16 Secteur;
	
	OutDbg("Cr�ation du fichier %s.",FileName);
	
	if (CreateEntry(FileName, FileInfo, 0x20, Buf, &FileEntry, &Secteur)) return 1;
	if (Floppy_Write(Secteur,Buf)) return 5;
	
	FatDuplicate();	// Recopie des secteurs de FAT modifi�s sur les FAT redondante.
	return 0;
}

// Cr�e un dossier dans le repertoire courant.
// 0 = ok.
char Fat12_CreateDir(char *DirName)
{
	TFat12_FileInfo FileInfo;
	u16 Cluster = 0, Secteur, ParentCluster = Util.DirClus;
	u8 Buf[512];
	TFat12_FileEntry *FileEntry;
	int i;

	OutDbg("Cr�ation du dossier %s.",DirName);

	if (CreateEntry(DirName, &FileInfo, 0x10, Buf, &FileEntry, &Secteur)) return 2;
	if (ClusterAlloc(&Cluster,1)) return  1;
	FileEntry->FstClusLo = Cluster;
	if (Floppy_Write(Secteur,Buf)) return 5;
	
	// Remplissage de 64 x 0. C'est le . et le .. tout vide.
	u32 *BufDw = (void*) Buf;
	for (i=0;  i < 16; ) BufDw[i++] = (u32) 0;
	
// !!! // Un 0 tout les 32 octet pour des ajout propre. Pas forc�ment utile...
	for (i=64; i < 512; Buf[i+=32] = 0);
	
	FileEntry = (TFat12_FileEntry*) ((u32*) Buf);
		
	FileEntry->FstClusLo = Cluster;
	FileEntry->Name[0] = '.';
	for (i=1; i < 11; FileEntry->Name[i++] = ' ');
	FileEntry->Attr = 0x10;
	
	FileEntry++;
	
	if (IsRootCluster(ParentCluster)) ParentCluster = 0;
	
	FileEntry->FstClusLo = ParentCluster;
	FileEntry->Name[0] = '.';
	FileEntry->Name[1] = '.';
	for (i=2; i < 11; FileEntry->Name[i++] = ' ');
	FileEntry->Attr = 0x10;
	
	if (Floppy_Write(ClustToSec(Cluster),Buf)) return 5;

	FatDuplicate();	// Recopie des secteurs de FAT modifi�s sur les FAT redondante.
	
	return 0;
}*/

// 1 = Ok.
char Fat32_ChangeDir(char *DirName)
{
	u32 Cluster;
	
	OutDbg("ChangeDir %s.",DirName);
			
	if (!FindEntry(DirName, 16, 8, &Cluster,NULL ,NULL))
	{
		Util.DirClus = Cluster;
		OutDbgColor(11,"ChangeDir %s result OK !",DirName);
		return 1;
	}
	OutDbgColor(12,"ChangeDir %s result nooooo !",DirName);
	return 0;
}

// 0 = no.
// 1 = ok.
char Fat32_IsDir(char *DirName)
{
	OutDbg("IsDir %s.",DirName);
	if (!FindEntry(DirName, 16, 8, NULL, NULL, NULL))
	{
		OutDbgColor(11,"IsDir %s result OK !",DirName);
		return 1;
	}
	OutDbgColor(12,"IsDir %s result nooooo !",DirName);
	return 0;
}

// 0 = no.
// 1 = ok.
char Fat32_IsFile(char *FileName)
{
	OutDbg("IsFile %s.",FileName);
	if (!FindEntry(FileName, 0xFF, 8+16, NULL, NULL, NULL)) return 1;
	return 0;
}
