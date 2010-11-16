// Bombela - vendredi 07 octobre 2005
// dimanche 09 octobre 2005
// mercredi 12 octobre 2005

// Classe de lecture des partitions.

// Connecte d'un coté un flux de donnée.
// Genre un HDD :D, et ça fait une liste propre des partitions.
// Avec max d'info.
// Et on peux sortir les infos que l'on veux par des fonctions.

#include "largefile.h"

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <stdint.h>
#include <list>

#include "util.h"

using namespace std;

// Class_Partition
// Le constructor de Class_Partition attend un pointeur sur un flux de type FILE.
// Lors de sa construction, Class_Partition fera sont marché dans le flux passé en paramètre.
// Les infos relative à une partition sont stoquée dans un S_Partition_Info.
// Pour lire le premier S_Partition_Info, il suffot d'accéder le class par un déférencement.
// Pour passer au second S_Partition_Info, il suffit de post d'incrémenter la class.
// Le dernier S_Partition_Info est fictif, il contient le type de partition 0.
// Ainsi, on peut savoir si on à parcourut tout les S_Partition_Info.
// Après avoir accéder au S_Partition_Info fictif, le prochain S_Partition_Info sera le premier.
// Pour forcer à revenir au premier S_Partition_Info. Il suffit de pré incrémenter la class.
// Ce principe de bidouiller avec les operators, c'est juste pour le fun ;)
// -------------------------------------------------------------------------------------------------
class Class_Partition
{
	
	// S_BootSector_FAT32
	// -------------------------------------------------------------------------------------------------
	#pragma pack (1) // Pas d'alignement automatique.
	typedef struct
	{
		uint8_t		jmp[3];
		char		OemName[8];
		
		uint16_t		BytePerSector;
		uint8_t		SectorPerCluster;
		uint16_t		ReservedSector;
		uint8_t		NumberOfFat;
		uint16_t		RootEntries;
		uint16_t		TotalSector;
		uint8_t		Media;
		uint16_t		FatSize;
		uint16_t		SectorPerTrack;
		uint16_t		HeadPerCylinder;
		uint32_t		NbHideSector;
		uint32_t		NbSectorBig;
		uint32_t		FatSizeBig;
		uint16_t		Flags;
		uint16_t		Version;
		uint32_t		RootCluster;
		uint16_t		InfoSector;
		uint16_t		RootBackupStart;
		uint8_t		Reserved[12];
		
		uint8_t		DriveNumber;
		uint8_t		Unused;
		uint8_t		ExtBootSignature;
		uint32_t		SerialNumber;
		char		LabelVolume[11];
		char		FileSystem[8];
	} S_BootSector_FAT32;
	
	typedef struct
	{
		union
		{
			struct
			{
				char		Name[8];
				char		Ext[3];
			} S_Entry_FAT32_Name;
			char FileName[11];
		};
		uint8_t		Attr;
		uint8_t		Reserved_NT;
		uint8_t		CreateTimeTenth;
		uint16_t		CreateTime;
		uint16_t		CreateDate;
		uint16_t		LastAccessDate;
		uint16_t		FirstCluster_hi;
		uint16_t		TimeModif;
		uint16_t		DateModif;
		uint16_t		FirstCluster;
		uint32_t		Size;
	} S_Entry_FAT32;
	
	// S_Partition_Item
	// -------------------------------------------------------------------------------------------------
	// La table de partion comporte 4 fois cette structure.
	// Elle contient les infos pour accéder au différentes
	// partition d'un disque dur (4 max). La première se 
	// trouve à l'offset 1BEh dans le MBR suivie des 3 autres.
	typedef struct
	{
		uint8_t	state;	// 80h = bootable, 0 = statique.
		uint8_t	head;	// Tête
		uint16_t	sectcyl;	// Secteur-Cylindre  
		uint8_t	type;	// 04h = Fat16 bits, 11d = Fat32bits
		uint8_t	headf;	// Tête de fin
		uint8_t	sectorf1;	// Secteur de fin  
		uint8_t	cyclef1;	// Cylindre de fin
		uint32_t	posi;	// Secteur de la partion par rapport au mbr
		uint32_t	length;	// Longeur de la partition en secteur   
	} S_Partition_Item;	
	
	#pragma pack () // Alignement automatique.
	
	// S_Partition_Info
	// -------------------------------------------------------------------------------------------------
	// Structure utilisée par Class_Partition pour stoquer les infos de chaques partitions.
	typedef struct
	{
		string Name; // Si on peut récup un nom de partition, sinon, c'est vide.
		uint64_t	StartSector; // C'est l'adresse de bootsector de la partition !
		uint8_t Type; // Le type de partition.
		bool Logical; // False si Primaire, true si logical.
	} S_Partition_Info;
	
	// Le type de liste de S_partition_Info
	typedef list<S_Partition_Info> T_Partition_List;
	
	
	// Variable de la class.
		FILE* hdd;
		T_Partition_List Partition_List;
		T_Partition_List::iterator Partition_List_Iterator;
		off_t ExtendedBaseSector;
	
	// Fonctions private.
		void AnalysePartitionList(off_t BootSector);
		bool ExtractPartitionInfo(off_t CurrentSector, S_Partition_Item* Partition_Item);
		string GetPartitionName_FAT32(off_t BootSector);
	
	//Fonction publique.
	public:
		Class_Partition(FILE* hdd);
		~Class_Partition(void);
		
		const S_Partition_Info operator*(void);		
		Class_Partition& operator++(int);
		Class_Partition& operator++(void);
};
