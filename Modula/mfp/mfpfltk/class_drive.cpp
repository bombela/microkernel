// Bombela - vendredi 07 octobre 2005
// dimanche 09 octobre 2005
// mercredi 12 octobre 2005
// mardi 01 novembre 2005
// 04/11/2005 - Detection des devices bloc sous linux.
// 03/11/2005 - Detection des disques dur sous winNT.

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
#include "class_drive.h"

using namespace std;

// operator*
// -------------------------------------------------------------------------------------------------
const string Class_Drive::operator*(void)
{
	if (Drive_List_Iterator != Drive_List.end())
	{
		return *Drive_List_Iterator;
	}
	else
	{	
		// Si on est à la fin de la liste des partitions.
		// On renvoie une chaîne vide.
		// Au passage, on remet la liste au début.
		
		Drive_List_Iterator = Drive_List.begin();
		
		return string();
	}
}

// operator++(int)
// -------------------------------------------------------------------------------------------------
Class_Drive& Class_Drive::operator++(int)
{
	if (Drive_List_Iterator != Drive_List.end()) Drive_List_Iterator++;
}

// operator++
// -------------------------------------------------------------------------------------------------
Class_Drive& Class_Drive::operator++(void)
{
	Drive_List_Iterator = Drive_List.begin();
}


// Constructor
// -------------------------------------------------------------------------------------------------
Class_Drive::Class_Drive()
{
	
#ifdef __linux__
	cout << "Linux -> ParsePartitonsFile()" << endl;
	ParsePartitonsFile();
#endif

#ifdef __WINNT__
	cout << "WinNT -> FindPhysicalDrive()" << endl;
	FindPhysicalDrive();
#endif
	
	Drive_List_Iterator = Drive_List.begin();
}

// Destructor
// -------------------------------------------------------------------------------------------------
Class_Drive::~Class_Drive(void)
{
	// Nettoyage de la liste.
	Drive_List.clear();
}

// ParsePartitonsFile
// Analyse /proc/partitions
// -------------------------------------------------------------------------------------------------
#ifdef __linux__
static inline int
_compare_digit_state (char ch, int need_digit)
{
	return !!isdigit (ch) == need_digit;
}

static int
_match_rd_device (const char* name)
{
	const char* pos;
	int state;

	/* exclude directory names from test */
	pos = strrchr(name, '/') ?: name;

	/* states:
	 * 	0	non-digits
	 * 	1	digits
	 * 	2	non-digits
	 * 	3	digits
	 */
	for (state = 0; state < 4; state++) {
		int want_digits = (state % 2 == 1);
		do {
			if (!*pos)
				return 0;
			if (!_compare_digit_state (*pos, want_digits))
				return 0;
			pos++;
		} while (_compare_digit_state (*pos, want_digits));
	}

	return *pos == 0;
}

bool Class_Drive::ParsePartitonsFile(void)
{
	FILE*		proc_part_file;
	int		major, minor, size;
	char		buf [512];
	char		part_name [256];
	char		dev_name [256];

	proc_part_file = fopen ("/proc/partitions", "r");
	if (!proc_part_file) return false;

	fgets (buf, 256, proc_part_file);
	fgets (buf, 256, proc_part_file);

	while (fgets (buf, 512, proc_part_file) && sscanf (buf, "%d %d %d %255s",
		&major, &minor, &size, part_name) == 4)
	{
		
		if (!_match_rd_device (part_name)
		    && isdigit (part_name [strlen (part_name) - 1]))
			continue;
			
		strcpy (dev_name, "/dev/");
		strcat (dev_name, part_name);
		Drive_List.push_back(string(dev_name));
		cout << dev_name << endl;
		
	}

	fclose (proc_part_file);
	return true;
}
#endif /* __linux__ */

// FindPhysicalDrive
// Test de FindPhysicalDrive0 à FindPhysicalDrive3.
// -------------------------------------------------------------------------------------------------
#ifdef __WINNT__
void Class_Drive::FindPhysicalDrive(void)
{
	// \\.\PHYSICALDRIVE0
	FILE* hDevice;
	string DevName;
	
	for (int DevNum = 0; DevNum < 4; DevNum++)
	{
		DevName = "\\\\.\\PHYSICALDRIVE" + IntToString(DevNum);
		if (hDevice = fopen(DevName.c_str(),"r"))
		{
			Drive_List.push_back(string(DevName));
			fclose(hDevice);
			cout << DevName << endl;
		}
	}
}
#endif
