// Bombela - vendredi 07 octobre 2005
// dimanche 09 octobre 2005
// mercredi 12 octobre 2005
// samedi, 24 juin 2006

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
#include <cstring>

#include "util.h"
#include "class_part.h"

using namespace std;

// operator*
// -------------------------------------------------------------------------------------------------
const Class_Partition::S_Partition_Info Class_Partition::operator*(void)
{
	if (Partition_List_Iterator != Partition_List.end())
	{
		return *Partition_List_Iterator;
	}
	else
	{	
		// Si on est à la fin de la liste des partitions.
		// On renvoie une fausse partition, avec un type de 0.
		// Au passage, on remet la liste au début.
		
		S_Partition_Info Partition_Info;
		Partition_Info.Name = "End of partition list !";
		Partition_Info.Type = 0;
		
		Partition_List_Iterator = Partition_List.begin();
		
		return Partition_Info;
	}
}

// operator++(int)
// -------------------------------------------------------------------------------------------------
Class_Partition& Class_Partition::operator++(int)
{
	if (Partition_List_Iterator != Partition_List.end()) Partition_List_Iterator++;
}

// operator++
// -------------------------------------------------------------------------------------------------
Class_Partition& Class_Partition::operator++(void)
{
	Partition_List_Iterator = Partition_List.begin();
}


// Constructor
// -------------------------------------------------------------------------------------------------
Class_Partition::Class_Partition(FILE* hdd)
{
	Class_Partition::hdd = hdd;
	AnalysePartitionList(0);
	Partition_List_Iterator = Partition_List.begin();
}

// Destructor
// -------------------------------------------------------------------------------------------------
Class_Partition::~Class_Partition(void)
{
	// Nettoyage de la liste.
	Partition_List.clear();
}

// AnalysePartitionList
// -------------------------------------------------------------------------------------------------
void Class_Partition::AnalysePartitionList(off_t BootSector)
{
	char Sector[512];
	S_Partition_Item* PI;
	S_Partition_Info* Result;

	// Déplacement...	
	fseeko(hdd,BootSector * 512,SEEK_SET);
	
	// Lecture du secteur.
	fread(Sector,512,1,hdd);
	
	// Récupération des partitions...	
	PI = (S_Partition_Item*) ( ((uintptr_t) &Sector) + 0x1BE);
	for (int i = 1; i <= 4; i++) ExtractPartitionInfo(BootSector,PI++);
}

// ExtractPartitionInfo
// -------------------------------------------------------------------------------------------------
// Presse un Partition_Item afin d'en sortir tout le jus.
// A besoin de connaitre l'emplacement courant -> CurrentSector.
// Rappelle AnalysePartitionList si une partition étendue est trouvée.
bool Class_Partition::ExtractPartitionInfo(off_t CurrentSector, S_Partition_Item* Partition_Item)
{	
	// Si c'est une partition étendue, on recurse :0)
	if (( (unsigned int) Partition_Item->type == 5) || ( (unsigned int) Partition_Item->type == 15))
	{
		if (ExtendedBaseSector == 0)
		{
			ExtendedBaseSector = Partition_Item->posi;
			AnalysePartitionList(ExtendedBaseSector);
		} else
		{
			AnalysePartitionList(ExtendedBaseSector+Partition_Item->posi);
		}
	} else
	{
		// Si c'est une entrée de partition non initializée, on oublie !
		if ( (unsigned int) Partition_Item->type != 0)
		{
			// Ah, enfin quelque chose à garder :0)
			
			S_Partition_Info Partition_Info;
			
			Partition_Info.StartSector = Partition_Item->posi;
			Partition_Info.Type = Partition_Item->type;
			if (CurrentSector == 0)
			{
				ExtendedBaseSector = 0;
				Partition_Info.Logical = false;
			} else
			{
				Partition_Info.Logical = true;
			}
			
			if ((Partition_Info.Type == 11) || (Partition_Info.Type == 12))
				Partition_Info.Name = GetPartitionName_FAT32(Partition_Item->posi);
			
			// Ajout dans la liste de partition.
			Partition_List.push_back(Partition_Info);
			
			return false;
		}
	}
	
}

// GetPartitionName_FAT
// Extrait le nom de volume d'une partition FAT32.
// -------------------------------------------------------------------------------------------------
string Class_Partition::GetPartitionName_FAT32(off_t BootSector)
{
	uint8_t Sector[512];
	S_BootSector_FAT32* BootSect = (S_BootSector_FAT32*) Sector;
	char VolName[12];
	
	VolName[11] = 0;
	
	cout << "GetPartitionName_FAT32 - BootSector : " << ExtendedBaseSector + BootSector << endl;

	fseeko(hdd,(ExtendedBaseSector + BootSector) * 512,SEEK_SET);
	fread(Sector,512,1,hdd);
	
	// Extraction du nom dans le bootsector directement.
	// Sera écrasé par la lecture dans la racine si entrée
	// de volume trouvé dans la racine.
	memcpy(VolName,BootSect->LabelVolume,11);
	
	cout << "BootSector VolumeName : " << VolName << endl;
	
	// Lecture de l'entrée VolumeName dans la racine.
	
	// Quelques variables utiles.
	uint32_t StartFat = ExtendedBaseSector + BootSect->ReservedSector + BootSector; 
	uint32_t StartData = StartFat + (BootSect->NumberOfFat * BootSect->FatSizeBig);
	uint32_t CurrentCluster = BootSect->RootCluster;
	uint32_t SectorPerCluster = BootSect->SectorPerCluster;
	
	// Carrément notre cluster entier...
	#pragma pack (1)
	#define MAX_FAT_ENTRY_IN_CLUSTER ((SectorPerCluster*512) / sizeof(S_Entry_FAT32))
	S_Entry_FAT32 FatEntry[MAX_FAT_ENTRY_IN_CLUSTER];
	#pragma pack ()
	
	cout << "Search Fat32 VolumeName Entry..." << endl;
	
	// Boucle de recherche de l'entrée FAT32.
	do
	{
		int EntryIndex	= 0;
		
		cout << "Root cluster analysing : " << CurrentCluster << endl;
		
		// Lecture du cluster.
		off_t SeekValue = (((CurrentCluster - 2) * SectorPerCluster) + StartData) * (off_t) 512;
		cout << "SeekValue in bytes : " << SeekValue << endl;
		fseeko(hdd,SeekValue,SEEK_SET);
		fread(FatEntry,512,SectorPerCluster,hdd);
		
		// Analyse de toute les entrée du cluster.
		do
		{
			if (FatEntry[EntryIndex].Attr & 8)
			{				
				memcpy(VolName,FatEntry[EntryIndex].FileName,11);
				cout << "Fat32 VolumeName Entry fouded at " << EntryIndex << endl;
				cout << "VolumeName Entry : " << VolName << endl;
				cout << "Stop with entry found - Fat32 Entry VolumeName used." << endl;
				return string(VolName);
			}
		} while (EntryIndex++ == MAX_FAT_ENTRY_IN_CLUSTER);
		
		// Cluster suivant...
		fseeko(hdd,(StartFat + (CurrentCluster / 128)) * 512,SEEK_SET);
		fread(Sector,512,1,hdd);
		CurrentCluster = *((intptr_t*) ((intptr_t) Sector) + (CurrentCluster % 128 *4));
		
	} while ((CurrentCluster < 0x0FFFFFF8) && (CurrentCluster != 0));
	
	if (CurrentCluster == 0) cout << "Stop with FREE cluster ! This is bad - BootSector VolumeName used." << endl;
		else cout << "Stop with EOF cluster - BootSector VolumeName used." << endl;
	
	return string(VolName);
}
