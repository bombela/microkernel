/*!
	\file mfpfltk.cpp
	\author Bombela
	\brief Fichier principal du programme.
	
	Mfpfltk permet de créer un BootSector préparé pour charger
	le Loader16 de Modula sur une partition Fat32.
	L'adresse de la partition Fat32 ainsi que le numéro de disque
	BIOS sont écrit en dur dans le BootSector.
	
	La fonction d'entrée est main().
	
	- samedi 01 octobre 2005
	- lundi 03 octobre 2005
	- samedi 08 octobre 2005
	- mercredi 12 octobre 2005
	- samedi 21 janvier 2006 - une première version utilisable.
	- 30.06.2006 01:13:01 - Compilation pour linux64.
*/

#include "largefile.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include <stdint.h>

#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Multi_Browser.H>

#include "window.cxx"

#include "util.h"
#include "class_drive.h"
#include "class_part.h"

using namespace std;

/*!
	\brief Contient le minimum pour retrouver le début d'une partition.
	
	Cette structure est utilisée dans la listbox afin d'associer à chaque
	élément les informations utiles pour la future création du BootSector.
*/
struct MiniPartInfo
{
	uint32_t	DeviceNum;		//!< Le numéro du disque dur qui contient la partition.
	uint32_t	BootSectorAdresse;	//!< L'adresse du secteur qui commence la partition.
	
};

// Fonctions en dessous.
void ScanPartition(string DevName, uint32_t DevNum);

//! Nom de l'executable.
string MfpFileName;

//! Début du programme.
int main(int argc, char **argv)
{
	MfpFileName = argv[0];
	
	Form_Main_Make();
	Form_Splash_Make();
	
	SetDefaultFloppyDev();

	// Affichage du spash screen.
	Form_Splash->show();
	
	if (argc > 1)
	{
		Splash_Browser->add("Scanning file...");
		ScanPartition(argv[1],0);
	}
	else
	{
		Splash_Browser->add("Scanning drive...");
		
		Class_Drive Drive;
		int i = 0;
		while ( (*Drive).size() > 0 )
		{
			ScanPartition((*Drive),i++);
			Drive++;
		}
	}
	
	Splash_Browser->add("Clic on Modula's logo to continue.");
	
	return Fl::run();
}

/*!
	\brief Génération et écriture du BootSector sur disquette ou fichier.
	\author Bombela

	À partir de la sélection de l'utilisateur,
	génère un BootSector adapté.
	Le BootSector est la la fin du programme lui même, il est
	donc d'abord récupéré, puis modifié pour ajouter l'adresse
	de la partition de boot ainsi que le numéro de disque
	de démarrage. Ensuite, le BootSector fraîchement créé
	est copié sur disquette ou dans un fichier.
*/
void MakeBootSector()
{
	int ID = ListBox->value();
	
	if ( (!ID) || (!ListBox->data(ID)) ) 
	{
		fl_message("Select modula partition compatible please");
		return;
	}
	
	char BootSector[512];
	char BootSector_Old[512];
	
	FILE* Mfp = fopen(MfpFileName.c_str(),"rb");
	
	if (!Mfp)
	{
		string msg = "Error to open " + MfpFileName + " !";
		fl_message(msg.c_str());
		return;
	}
	
	fseek(Mfp,-512,SEEK_END);
	fread(BootSector,1,512,Mfp);
	fclose(Mfp);
	
	// Préparation du BootSector...
	
	BootSector[512-7] = ((MiniPartInfo*) ListBox->data(ID))->DeviceNum + 0x80;
	uint32_t* dd = (uint32_t*) &BootSector[512-6];
	*dd = ((MiniPartInfo*) ListBox->data(ID))->BootSectorAdresse;
	
	string FloppyPath = Input_FloppyDev->value();
	FILE* Floppy = fopen(FloppyPath.c_str(),"r+b");
	if (!Floppy)
	{
		string msg = "Error to open " + FloppyPath + " !";
		fl_message(msg.c_str());
		return;
	}
	
	// Lecture du BootSector
	fseek(Floppy,0,SEEK_SET);
	fread(BootSector_Old,1,512,Floppy);
	
	// Mofif du BootSector
	for (int i = 11; i < 38; i++) BootSector[i] = BootSector_Old[i];
	
	// Écriture du BootSector.
	
	fseek(Floppy,0,SEEK_SET);
	if (!fwrite(BootSector,1,512,Floppy))
	{
		string msg = "Error to write in " + FloppyPath + " ! :'(";
		fl_message(msg.c_str());
		return;
	}
	
	fclose(Floppy);
	
	fl_message("BootSector maked !");
}

//! Simple fonction qui attribue le nom de périphérique disquette par défaut.
void SetDefaultFloppyDev()
{
	#ifdef __linux__
	Input_FloppyDev->value("/dev/fd0");
	#endif
	
	#ifdef __WINNT__
	Input_FloppyDev->value("//./A:");
	#endif
}

/*!
	\brief Ouvre la boite de selection de fichier.
	
	Ouvre une boite de dialogue permettant la selection d'un fichier.
	De là même façons, cela permet de selectionner un lecteur disquette, un disque dur etc.	
*/
void Button_Browse()
{
	Fl_File_Chooser Browser("", "Image file or device (*)", 0, "MFPFLTK - Bombela");
	Browser.preview(0);
	
	Browser.show();
	while(Browser.shown()) Fl::wait();
	
	Input_FloppyDev->value(Browser.value());
}

//! Fonction qui affiche la fenêtre principale.
void NextSplash()
{
	Form_Main->show();
}

//! Détecte toutes les partitions de tous les disques durs.
void ScanPartition(string DevName, uint32_t DevNum)
{
	FILE* hdd;
	
	if (hdd = fopen(DevName.c_str(),"rb"))
	{
		Class_Partition Part(hdd);
		
		string Scanning = "Scanning '" + DevName + "' ...";
		Splash_Browser->add(Scanning.c_str());
		
		int parti = 1;
		while ( (*Part).Type > 0 )
		{
			string o;
			string type = IntToString((int) (*Part).Type);
			string partnum = IntToString(parti);
			
			o = DevName + "\tN° : " + partnum + "\t";
			o += ((*Part).Logical?"Logical  ":"Principal");
			o += "\t Type : " + type + "\t";
			
			struct MiniPartInfo* MPI = NULL;
			
			if (((*Part).Type == 11) || ((*Part).Type == 12))
			{
				MPI = new(MiniPartInfo);
				MPI->DeviceNum = DevNum;
				MPI->BootSectorAdresse = ((*Part).StartSector);
				o = "@C150" + o + "(FAT32)";
			}
			else
			{
				o += "       ";
			}
			o += " \t" + (*Part).Name;
			
			ListBox->add(o.c_str(),MPI);
			
			Part++;
			parti++;
		}
		
		fclose(hdd);
		
	} else
	{
		string Error = "Oh shit... Error opened '" + DevName + "' ... Are you root ?";
		Splash_Browser->add(Error.c_str());
	}
}
