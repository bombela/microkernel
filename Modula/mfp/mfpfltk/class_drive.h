// Bombela - dimanche 30 octobre 2005
// lundi 31 octobre 2005

// Classe de détection de périphérique de bloc.

// Donne une liste de string vers des périphérique de bloc.

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

// Class_Drive
// -------------------------------------------------------------------------------------------------
class Class_Drive
{
	// La liste de path de drive.
		typedef list<string> T_Drive_List;
	
	
	// Variable de la class.
		T_Drive_List Drive_List;
		T_Drive_List::iterator Drive_List_Iterator;
	
	// Fonctions private.
	#ifdef __linux__
		bool ParsePartitonsFile(void);
	#endif
	
	#ifdef __WINNT__
		void FindPhysicalDrive(void);
	#endif
	
	//Fonction publique.
	public:
		Class_Drive(void);
		~Class_Drive(void);
		
		const string operator*(void);
		Class_Drive& operator++(int);
		Class_Drive& operator++(void);
};
