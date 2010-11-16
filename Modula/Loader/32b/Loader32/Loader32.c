	/*
	 *	Loader, partie 32 bits.
	 *
	 *	Bombela.
	 *	27/06/2004
	 *	03/03/2005
	 */
	
	#define Version "0.0.1 - "__DATE__
	
	//////////////////////////////////////////////
	
	#include <Debug_Start.h>
	#ifdef DEBUG_LOADER32
	#define DEBUG_NAME "LOADER32"
	#endif
	#include <Debug_End.h>
	
	#include <Types.h>
	#include <String.h>
	#include <Arg.h>
	#include <OutText.h>
	#include <Pic.h>
	#include <Int.h>
	#include <Ide.h>
	#include <Dma.h>
	#include <Timer.h>
	#include <Floppy.h>
	#include <Fat12.h>
	#include <Date.h>
	#include <Fat32.h>
	#include <Mem.h>
	#include <Elf.h>
	
	// D�claration des fonctions plus bas.
	// -------------------------------------------
	void Loader32();
	void Stop();
	// -------------------------------------------
	
	// Loader32 ! Entry point !
	// -------------------------------------------
	void Loader32()
	{
		u8 Disk;
		unsigned int BootSect;
		
		OutT_Init();
	
		OutT("\nModula Loader32-"Version);
		
		OutDbg("-- Loader32 compilation DEBUG --");
		OutDbg("Mode 32 bits !");
	
		// R�cup�ration du disque et du LBA
		// dans le bootsector de lancement.
		Disk =     *((char*) (0x7C00+505)) - 0x80;
		BootSect = *((int*)  (0x7C00+506));
	
		OutDbg("Disque et adresse du bootsector de la partition de chargement :");
		OutDbg("Disque: %u, Adresse: %h.",Disk,BootSect);
		// --------------------------------------------------------------------
	
		
		OutDbg("Activation du log dans OutText...");
		if (OutT_LogStart())
		{
			OutT_SetAttr(12);
			OutT("\nLe log ne s'est pas activ� !");
			Stop();
		}
		OutDbgColor(10,"Log activ�.");

		OutDbg("Initialisation du pilote du PIC...");
		if (Pic_Init() > 0)
		{
			OutT_SetAttr(12);
			OutT("\nLe pilote du PIC ne s'initialise pas !");
			Stop();
		}
		OutDbgColor(10,"PIC initialis�..");
	
		OutDbg("Initialisation du pilote d'INT...");
		if (Int_Init() > 0)
		{
			OutT_SetAttr(12);
			OutT("\nLe pilote d'interruptions ne s'initialise pas !");
			Stop();
		}
	
		OutDbgColor(10,"INT initialis�.");
	
		OutDbg("Initialisation du pilote de timer...");
		if (Timer_Init() > 0)
		{
			OutT_SetAttr(12);
			OutT("\nLe pilote du timer ne s'initialise pas !");
			Stop();
		}
		OutDbgColor(10,"TIMER initialis�.");

		OutDbg("Initialisation du pilote DMA...");
		if (Dma_Init() > 0)
		{
			OutT_SetAttr(12);
			OutT("\nLe pilote DMA ne s'initialise pas !");
			Stop();
		}
		OutDbgColor(10,"DMA initialis�.");
	
		OutDbg("Initialisation du driver disquette...");
		if (Floppy_Init())
		{
			OutT_SetAttr(12);
			OutT("\nLe pilote floppy ne s'initialise pas !");
			Stop();
		}
		OutDbgColor(10,"FLOPPY initialis�..");	
	
		OutDbg("Initialisation du driver FAT12...");
		if (Fat12_Init())
		{
			OutT_SetAttr(12);
			OutT("\nLe pilote FAT12 ne s'initialise pas !");
			Stop();
		}
		OutDbgColor(10,"FAT12 initialis�.");
		
		OutDbg("Recherche du dossier MODULA...");	
		if (!Fat12_IsDir("MODULA     "))
		{
			OutDbg("Non trouv�... cr�ation...");
			if (Fat12_CreateDir("MODULA     "))
			{
				OutT_SetAttr(12);
				OutT("\nImpossible de cr�er le dossier Modula dans la disquette...");
				Stop();
			}
		}
		if (!Fat12_ChangeDir("MODULA     "))
		{
			OutDbg("Erreur impossible d'ouvrir le dossier...");
			Stop();
		}
		OutDbg("Entr� dans le dossier MODULA.");

		OutDbg("Initialisation du pilote IDE...");
		if (IDE_Init() > 0)
		{
			OutT("\nLe pilote IDE ne s'initialise pas !");
			Stop();
		}
		OutDbgColor(10,"IDE initialis�.");
		
		OutDbg("Initialisation du driver FAT32...");
		if (Fat32_Init(&IDE_GetCtrl()->Devices[Disk],BootSect))
		{
			OutT_SetAttr(12);
			OutT("\nLe pilote FAT32 ne s'initialise pas !");
			Stop();
		}
		OutDbgColor(10,"FAT32 initialis�.");
	
		OutDbg("Recherche du dossier MODULA...");
		if (!Fat32_ChangeDir("MODULA     "))
		{
			OutDbg("Erreur impossible d'ouvrir le dossier Modula sur la partition...");
			Stop();
		}
		OutDbg("Entr� dans le dossier MODULA.");
		
		OutDbg("Initialisation du module m�moire...");
		if (Mem_Init())
		{
			OutT_SetAttr(12);
			OutT("\nLe module de m�moire physique ne s'initialise pas !");
			Stop();
		}
		OutDbgColor(10,"MEM initialis�.");
		
		OutDbg("Allocation des zones m�moire utilis�e.");
		OutDbg("--->>> Pour faire simple, allocation du premier MO !");
		{
			unsigned int i;
			for (i=4096; i < (1024*1024); i += 4096) Mem_Alloc_Adress(i,0);
		}
		OutDbg("Allocation termin�e.");
	
		{
			OutDbg("Allocation d'une petite page pour le plaisir !");
			void *Page = Mem_Alloc();
			OutDbg("Voici son adresse : %h.",(unsigned int) Page);

			OutDbg("Aller... encore une autre !");
			void *Page2 = Mem_Alloc();
			OutDbg("Et l'adresse de cette deuxi�me page : %h.",(unsigned int) Page2);

			OutDbg("On lib�re la premi�re page.");
			Mem_Free_Adress((unsigned int) Page);
			OutDbg("On lib�re la deuxi�me !");
			Mem_Free_Adress((unsigned int) Page2);
		}
		
		/*OutDbg("Initialisation du module Elf...");
		if (Elf_Init())
		{
			OutT_SetAttr(12);
			OutT("\nLe module ELF ne s'initialise pas !");
			Stop();
		}
		OutDbgColor(10,"Elf initialis�.");*/
		
		/*{
			#define FILE_BUF_SIZE (1024*32)
			TFat32_FileInfo File;
			
			OutDbg("Ouverture de salut.txt");
			if (!Fat32_OpenFile("SALUT   TXT",&File))
			{
				u8 buf[FILE_BUF_SIZE];
				int Count,i;
				
				OutDbgColor(5,"Taille du fichier %u.",File.Size);
				OutDbg("Contenu de salut.txt (Lut par paquet de 32ko)\n");
				for (Count = 0; Count < (File.Size/FILE_BUF_SIZE); Count++)
				{
					Fat32_ReadFile(&File,FILE_BUF_SIZE,buf);
					for (i=0; i < FILE_BUF_SIZE;) OutT_PutChar(buf[i++]);
				}
				if (File.Size%FILE_BUF_SIZE)
				{
					Fat32_ReadFile(&File,(File.Size%FILE_BUF_SIZE),buf);
					for (i=0; i < (File.Size%FILE_BUF_SIZE);) OutT_PutChar(buf[i++]);
				}
				OutDbg("Tout est affich�.");
				
				Fat32_CloseFile(&File);
				
			} else OutDbg("Erreur...");
		}*/
		
		// Cherche la table ACPI et la stoque dans dump.bin
		/*{
			TFat12_FileInfo File;
			int i = 0;
			int max;
			boolean found = false;
			
			OutT_SetAttr(15); // Un beau blanc pour tout nos tests ;)
			OutT("\nCherche le RSD PTR : le d�but de la table ACPI...");
			for (i=640*1024; i < 1024*1024; i++)
			{
				if ( *((u32*)i) == 541348690)
				{
					found = true;
					OutT(" trouv�e � %u.",i);
					break;
				}
			}		
			
			max = i + 20000;
			if ( (found) && (!Fat12_CreateFile("DUMP    BIN",&File)) )
			{
				OutT("\nCopie de 20ko (Par paquet de 2ko)");
				for (; i < max; i+=2048)
				{
					OutT(".");					
					Fat12_WriteFile(&File,2048,(void*) i);
				}
				Fat12_CloseFile(&File);
				OutT(" Termin�");
			} else
			{
				if (found) OutT("\nErreur de cr�ation du fichier..."); else OutT(" Pas trouv�.");
			}
		}*/
	
		// Quelques infos.
		{
			TDate_Hour Hour;
			TDate_Date Date;
			
			OutT_SetAttr(11);
			
			OutT("\nVitesse du premier CPU : %u Mhz.",Timer_GetCpuSpeed());
			
			Date_GetCurrentDate(&Date);
			Date_GetCurrentHour(&Hour);
			OutT("\nNous sommes le %U2/%U2/%u et il est %U2:%U2:%U2.",
					Date.Day,Date.Month,Date.Year,Hour.Hour,Hour.Min,Hour.Sec);
		}
		
		Stop();
	}
	
	// Arr�t !
	// -------------------------------------------
	
	void Stop()
	{
		OutDbg("Arret de Modula...");
		OutT_LogStop();
		
		Floppy_Release();
		Pic_Release();
		Int_Release();
		
		OutT_SetAttr(14);
		OutT("\nSYSTEM HALTED - Ok... On panique pas...");
		OutT_Release();
		
		asm("hlt");
		while (1);
	}
