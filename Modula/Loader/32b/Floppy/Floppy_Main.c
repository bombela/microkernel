/*
 *	Petit pilote disquette..
 *
 *	Bombela.
 *	29/09/2004
 *	11/03/2005
 *	29/01/2006 - Correction d'une erreur monstruseuse... 
 *			La vérification du déplacement des tetes étais 
 *		if (St0 != 0x20) || (FdcTrack != Track)) et doit bien
 *		sur  etre if (((St0 & 0xE0) != 0x20) || (FdcTrack != Track))
 *		une petite erreur qui à demandée 16h de debuggage...
 */

//////////////////////////////////////////////

#include <Debug_Start.h>
#ifdef DEBUG_FLOPPY
#define DEBUG_FLOPPY_RW
#define DEBUG_FLOPPY_BUF
#define DEBUG_NAME "FLOPPY  "
#endif
#include <Debug_End.h>

#include <Types.h>
#include <Floppy.h>
#include <Io.h>
#include <OutText.h>
#include <Timer.h>
#include <Pic.h>
#include <Int.h>
#include <Dma.h>

#include "Floppy_Cmd.h"

// --------------------------------------------------------------

extern void Floppy_Int();

// Alligné sur 64ko obligé !
#define		SecDma	0x50000

// Pour gérer les secteurs mis en mémoires...
#define MaxSecBuf 36

u8		SecBufPriority[MaxSecBuf];
u16		SecBufLba[MaxSecBuf];
u8		SecBuf[MaxSecBuf][512];

volatile u8 Floppy_IntCount;
static u8 CmdResult[7];
static u8 St0;
static u8 FdcTrack;

void SendData(u8 Value)
{
   volatile u8 msr;
   u16 tmo;

   for (tmo = 0;tmo < 130;tmo++)
   {
      msr = Inb(FDC_MainStatus);
      if ((msr & 0xc0) == 0x80)
      {
	 	Outb(FDC_Data,Value);
	 	return;
      }
      Timer_SleepU(1);
   }
}

u8 GetData()
{
   volatile u8 msr;
   u16 tmo;

   for (tmo = 0;tmo < 130;tmo++)
   {
      msr = Inb(FDC_MainStatus);
      if ((msr & 0xd0) == 0xd0) return Inb(FDC_Data);
      Timer_SleepU(1);
   }
   return -1;
}

// Si CheckInt = true, récupération du CheckInt
// dans St0 et FdcTrack.
char WaitInt(boolean CheckInt)
{
	u32 i = 0;

	OutDbg("Attend l'int%s.",CheckInt?" avec ChekInt":"");

	 // Attend l'int.
	while (!Floppy_IntCount)
	{
		i++;
		if (i == 1000000) // Timeout !!
		{
			OutDbg("TimeOut !");
			return 1;
		}
		Timer_SleepU(1);
	}

	Floppy_IntCount = 0;

	// Lecture des octets de résultat.
	i = 0;
	while ((i < 7) && (Inb(FDC_MainStatus) & (1<<4)))
	{
		CmdResult[i++] = GetData();
		OutDbg("Result %u: %b1",i-1,CmdResult[i-1]);
	}

	if (CheckInt)
	{
		SendData(CMD_CHECKINT);
		St0 = GetData();
		FdcTrack = GetData();
		OutDbg("CheckInt : [St0 : %b1], [FdcTrack : %u].",St0,FdcTrack);
	}

	return 0;
}

char Floppy_Seek(u32 Track)
{
	OutDbg("Déplacement sur la piste %u...",Track);
	if (FdcTrack == Track)
	{
		OutDbg("Déjà sur la bonne piste !");
		return 0;
	}

	// Envoie la commande...
	SendData(CMD_SEEK);
	SendData(0);
	SendData(Track);

	// Attend lancement de la commande.
	// En même temps, FdcTrack est mis à jours.
	if (WaitInt(true)) return 1;

	// Test si réussit !
	if (((St0 & 0xE0) != 0x20) || (FdcTrack != Track))
	{
		OutDbg("Erreur de déplacement !");
		return 1;
	}

	return 0;
}

char Floppy_Calibrate()
{
	OutDbg("Recalibration...");

	// Envoie la commande...
	SendData(CMD_RECAL);
	SendData(0);

	// Attend la fin.
	if (WaitInt(true)) return 1;

	if ((St0 != 0x20) || (FdcTrack != 0))
	{
		OutDbg("Erreur de calibration !");
		return 3;
	}

	return 0;
}

char Floppy_Reset(void)
{
	OutDbg("Reset du FDC...");
	
	// Stop le moteur et stop les int. 
	Outb(FDC_Dor,0);
	
	// 500K/s
	Outb(FDC_Config,0);
	
	// Activation des interruptions.
	Outb(FDC_Dor,0x0c);
	
	// Spécification des timing
	SendData(CMD_SPECIFY);
	SendData(0xdf);  // SRT = 3ms, HUT = 240ms
	SendData(0x02);  // HLT = 16ms, ND = 0
	
	return 0;
}

char Floppy_Init()
{
	int i;

	OutDbg("-- Pilote disquette A DEBUG --");

	for (i=0; i < MaxSecBuf; i++) SecBufPriority[i] = 0;

	OutDbg("Connexion de l'IRQ13 (floppy) à la fonction INT floppy...");
	Int_SetInt(Pic_IrqToInt(13),Floppy_Int);

 	OutDbg("Activation de l'IRQ13...");
	Pic_IrqMask(13, true);

	// Très important, sert à WaitInt.
	Floppy_IntCount = 0;
	
	if (Floppy_Reset())	return 4;
	
	OutDbg("Detection de la version du FDC...");
	SendData(CMD_VERSION);
	if (GetData() == 0x80)
	{
		OutDbg("FDC Intel 82072/7 trouvé. Non géré car trop vieux !")
		return 1;
	}
	else OutDbg("FDC NEC765 trouvé.");

	OutDbg("Selectionne et allume le moteur du lecteur A...");
	Outb(FDC_Dor,0x1C);
	
	// Attend lancement de la commande.
	if (WaitInt(true)) return 2;
	
	if (Floppy_Calibrate()) return 4; // Pour aligner les têtes sur les pistes de la disquette.
	if (Floppy_Seek(1))	return 5; // Pour placer sur le premier secteur, ce qui initialise le "drive change".
	
	return 0;
}

void LbaToChs(u32 Lba,u8 *Head,u8 *Track,u8 *Sector)
{
   *Head = (Lba % (18 * 2)) / (18);
   *Track = Lba / (18 * 2);
   *Sector = Lba % 18 + 1;
   OutDbg("LbaToChs : %u --> %u, %u, %u.",Lba,*Head,*Track,*Sector);
}

char Floppy_ReadWrite(u32 Lba,void* Buf,boolean Write)
{
	u16 i,indx;
	u8 Head,Track,Sector;
	u8 min = 0;

	OutDbg("Opération %s.",Write?"d'écriture":"de lecture");
	
	if (Lba > 2880)
	{
		OutDbgColor(12,"Nan mais essaye de dépasser 2880 secteurs pour voir !!!");
		OutDbgColor(12,"%i secteurs c'est trop !",Lba);
		return 8;
	}

	if (Inb(FDC_Dir) & 0x80)
	{
		OutDbg("La disquette à changé !");
		OutDbg("On continue à l'arrache...");
		//return 3;
	}

	// Copie le buffer utilisateur dans le buffer DMA.
	if (Write)
		for (i = 0; i < 512; i+=4) *((u32*) (SecDma + i)) = *((u32*) (Buf+i));


	OutDbg("Cherche si le secteur %u est dans un buffer...",Lba);
	for (indx=0; indx < MaxSecBuf; indx++)
		if ((SecBufPriority[indx] > 0) && (SecBufLba[indx] == Lba))
		{
			if (Write)
			{
				// Si le secteur est dans un buffer, il faut le virer.
				#ifdef DEBUG_FLOPPY_BUF
				OutT_SetAttr(10);
				OutT("\n [FLOPPY  ] : Secteur %u trouvé dans le buffer %u, effacement...",Lba,indx);
				#endif

				SecBufPriority[indx] = 0;
				break;
			}
			else
			{
				// Si le secteur est dans un buffer, pas la peine de le relire.
				#ifdef DEBUG_FLOPPY_BUF
				OutT_SetAttr(10);
				OutT("\n [FLOPPY  ] : Secteur %u trouvé dans le buffer %u, recopie...",Lba,indx);
				#endif

				if (SecBufPriority[indx] > 0) SecBufPriority[indx]--;
				for (i = 0; i < 512; i+=4) *((u32*) (Buf+i)) = *((u32*) &SecBuf[indx][i]);
				return 0;
			}
		}
	OutDbg("%s...",Write?"Eécriture":"Lecture");

	LbaToChs(Lba,&Head,&Track,&Sector);

	if (Floppy_Seek(Track)) return 1;

	OutDbg("Prépare la requête DMA...");
	if (!Write)
		Dma_SetRequest(2,SecDma / 0x10000,512,false);  // A 64ko. Lecture.
	else
		Dma_SetRequest(2,SecDma / 0x10000,512,true); // A 64ko. Ecriture

	OutDbg("Envoie la commande...");
	if (!Write)	SendData(CMD_READ); else SendData(CMD_WRITE);
	SendData(Head << 2); // Head, Device.
	SendData(Track); // Track (Cylindre).
	SendData(Head); // Head.
	SendData(Sector); // Sector.
	SendData(2); // 512 u8s/sector.
	SendData(18);// 18 sectors/track.
	SendData(0x1b); // Gap 1.44 Mo.
	SendData(0xff); // Not used.

	// Attend la fin de la commande.
	if (WaitInt(false)) return 2;

	if (CmdResult[0] & 0xC0)
	{
		OutDbgColor(12,"Erreur %s !",Write?"d'écriture":"de lecture");
		return 3;
	}



	// Cherche le buffer de secteur le plus vieux...
	OutDbg("Cherche le plus vieux buffer pour le remplacer...");
	indx = 0;
	for (i = 0; i < MaxSecBuf; i++)
	{
		if (SecBufPriority[i] == 0)
		{
			indx = i;
			break;
		}
		else
		if (SecBufPriority[i] > min)
		{
			min = SecBufPriority[i];
			indx = i;
		}

		// Pour "viellir" le contenu des buffers...
		if ((SecBufPriority[i] > 0) && (SecBufPriority[i] < 255)) SecBufPriority[i]++;
	}

	#ifdef DEBUG_FLOPPY_BUF
	OutT_SetAttr(9);
	OutT("\n [FLOPPY  ] : Remplacement du buffer %u par le secteur %u...",indx,Lba);
	#endif

	SecBufPriority[indx] = 1; // Il est tout jeunnot...
	SecBufLba[indx] = Lba;
	// Copie du secteur lut/écrit dans le buffer de secteur...
	for (i = 0; i < 512; i+=4) *((u32*) &SecBuf[indx][i]) = *((u32*) (SecDma + i));



	if (!Write)
		// Copie du secteur lut dans le buffer utilisateur.
		for (i = 0; i < 512; i+=4) *((u32*) (Buf+i)) = *((u32*) (SecDma + i));

	return 0;
}

char Floppy_Read(u32 Lba,void* Buf)
{
	return Floppy_ReadWrite(Lba,Buf,false);
}

char Floppy_Write(u32 Lba,void* Buf)
{
	return Floppy_ReadWrite(Lba,Buf,true);
}

char Floppy_Release()
{
	OutDbg("Selectionne et éteint le moteur du lecteur A...");
	Outb(FDC_Dor,12);

	OutDbg("Pilote floppy released...");

	return 0;
}
