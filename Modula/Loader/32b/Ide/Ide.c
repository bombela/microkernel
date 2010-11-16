/*
 *	Petit pilote IDE assurant seulement la lecture.
 *
 *	Bombela.
 *	30/06/2004
 *	29/09/2004
 *	17/03/2006 - Lecture de plusieurs secteurs de magnière logicielle.
 */

//////////////////////////////////////////////
#include <Debug_Start.h>
#ifdef DEBUG_IDE
#define DEBUG_NAME "IDE     "
static int Time = 0; // Variable utilisée pour le débug.
#endif
#include <Debug_End.h>

#include <Types.h>
#include <Ide.h>
#include <Io.h>
#include <OutText.h>
#include <Timer.h>

#include "ATA.h"

// --------------------------------------------------------------

static TIDE_Controleur Ctrls[2] =
{
	{
		.IoAddr	=	0x1F0,
		.State	=	IDE_CTRL_NOT_PRESENT,
		.Devices =
		{
			[0] =
			{
				.Type = IDE_DEVICE_MASTER,
				.State = IDE_DEVICE_NOT_PRESENT,
				.Ctrl = &Ctrls[0],
			},
			[1]
			{
				.Type = IDE_DEVICE_SLAVE,
				.State = IDE_DEVICE_NOT_PRESENT,
				.Ctrl = &Ctrls[0],
			},
		},
	},
	{
		.IoAddr	=	0x170,
		.State	=	IDE_CTRL_NOT_PRESENT,
		.Devices =
		{
			[0] =
			{
				.Type = IDE_DEVICE_MASTER,
				.State = IDE_DEVICE_NOT_PRESENT,
				.Ctrl = &Ctrls[1],
			},
			[1]
			{
				.Type = IDE_DEVICE_SLAVE,
				.State = IDE_DEVICE_NOT_PRESENT,
				.Ctrl = &Ctrls[1],
			},
		},
	}
};

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// Selectionne le DEVICE d'un controleur.
// DRIVE = 0 pour Master et 1 pour Slave.
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void IDE_SetDrive(TIDE_Controleur *Ctrl, u8 Drive)
{
	Outb(Ctrl->IoAddr + ATA_DRIVE,(!Drive) ? ATA_D_MASTER : ATA_D_SLAVE);
	Timer_SleepU(1);
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// Récupère le statut d'un DEVICE.
// DRIVE = 0 pour Master et 1 pour Slave.
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
u8 IDE_GetDeviceStatut(TIDE_Device *Device)
{
	IDE_SetDrive(Device->Ctrl,Device->Type);
	return Inb(Device->Ctrl->IoAddr + ATA_STATUS);
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// Detecte le state d'un device.
// Après un reset seulement !
// Succès : 0 et autre pour une erreur.
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
char IDE_InitDevice(TIDE_Device *Device)
{
	u8 Signs[5];

	OutDbg("Detection du type du DEVICE %s...",(Device->Type == IDE_DEVICE_MASTER) ? "MASTER" : "SLAVE" );

	IDE_SetDrive(Device->Ctrl,Device->Type); // Selectionne le DEVICE.

	// Récupère les signatures.

	Signs[0] = Inb(Device->Ctrl->IoAddr + ATA_SECTOR_COUNT);
	Signs[1] = Inb(Device->Ctrl->IoAddr + ATA_LBA_0);
	Signs[2] = Inb(Device->Ctrl->IoAddr + ATA_LBA_1);
	Signs[3] = Inb(Device->Ctrl->IoAddr + ATA_LBA_2);
	Signs[4] = Inb(Device->Ctrl->IoAddr + ATA_LBA_3);

	OutDbg(">> Signature: [%u],[%u],[%u],[%u],[%u]",Signs[0],Signs[1],Signs[2],Signs[3],Signs[4]);

	// Un device valide ?
	if ( (Signs[0] == 1) && (Signs[1] == 1) && !(Signs[4] & ~0xB0) )
	{
		// Un ATAPI ?
		if ( (Signs[2] == 0x14) && (Signs[3] == 0xEB) )
		{
			// C'est un ATAPI.
			OutDbg("-> ATAPI CDROM.");
			Device->State = IDE_DEVICE_ATAPI;
			return 0;
		}

		// Un ATA ?
		if ( !Signs[2] && !Signs[3] )
		{
				// C'est un ATA.
				OutDbg("-> ATA DISQUE DUR.");
				Device->State = IDE_DEVICE_ATA;
				return 0;
		}

	}

	OutDbg("-> AUCUN DEVICE.");

	return 0;
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// Initialize un contrôleur IDE.
// Renvoi 0 (OK) ou autre (ERR).
// Attention !
// Si il n'y a pas de DEVICE, renvoie quand même 0.
// State est dans ce cas à IDE_CTRL_PRESENT.
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#define TIMEOUT_COUNT       30000
#define TIMEOUT_RESET_COUNT 10
char IDE_InitCtrl(TIDE_Controleur *Ctrl)
{
	int Status0, Status1;
	int TimeOut, TimeOutReset;
	u8 Mask = 0;

	// Statut du controleur.
	OutDbg("Recupere le statut du controleur (IoAddr: %h).",Ctrl->IoAddr);

	Status0 = IDE_GetDeviceStatut(&Ctrl->Devices[0]);
	Status1 = IDE_GetDeviceStatut(&Ctrl->Devices[1]);

	OutDbg(">> Statut0 = %u, Statut1 = %u.",Status0,Status1);

	// Controleur absent ?
	if ( (Status0 & ATA_S_BSY) && (Status1 & ATA_S_BSY) )
	{
		OutDbg("Pas de controleur.");
		return 0;
	}

	if ( (Status0 & 0xF8) && !(Status0 & 0x20) ) Mask |= 1;
	if ( (Status1 & 0xF8) && !(Status1 & 0x20) ) Mask |= 2;

	if (!Mask)
	{
		OutDbg("Aucun DEVICE connecte.");
		return 0;
	}

	for (TimeOutReset = 0; TimeOutReset < TIMEOUT_RESET_COUNT; TimeOutReset++)
	{

		OutDbg("    Reset du controleur (Essai %u/%u)...",TimeOutReset+1,TIMEOUT_RESET_COUNT);

		// Selectino du Master pour le reset.
		IDE_SetDrive(Ctrl,0);
		// Envoie la commande de reset.
		Outb(Ctrl->IoAddr + ATA_DEVICE_CONTROL, ATA_A_nIEN | ATA_A_RESET);
		Timer_SleepU(1);
		Outb(Ctrl->IoAddr + ATA_DEVICE_CONTROL, ATA_A_nIEN);
		Timer_SleepU(1);
		Outb(Ctrl->IoAddr + ATA_DEVICE_CONTROL, ATA_A_nIEN | ATA_A_4BIT);
		Timer_SleepU(1);

		// Attend la fin du reset.
		for (TimeOut = 0; TimeOut < TIMEOUT_COUNT; TimeOut++)
		{
			// Recupère le statut du Master.
			if (Mask & 1) Status0 = IDE_GetDeviceStatut(&Ctrl->Devices[0]);
			// Et celui du slave.
			if (Mask & 2) Status1 = IDE_GetDeviceStatut(&Ctrl->Devices[1]);

			//OutDbg("    Status0 = %b1, Status1 %b1.",Status0,Status1);

			if ( ( !(Mask & 1) || !(Status0 & ATA_S_BSY) ) &&
				 ( !(Mask & 2) || !(Status1 & ATA_S_BSY) ) ) break;

			Timer_SleepU(1);
		}

		#ifdef DEBUG_IDE
		Time += TimeOut;
		#endif

		// Si not TimeOut, teste les DEVICE..
		if (TimeOut < TIMEOUT_COUNT)
		{
			// Si DEVICE ok, bye.
			if ( ( !(Mask & 1) || !(Status0 & ATA_S_DF) ) &&
		    	 ( !(Mask & 2) || !(Status1 & ATA_S_DF) ) ) break;
		}
	}

	OutDbg("    Reset termine. (Dernier compteur timeout : %u / %u)",TimeOut,TIMEOUT_COUNT);

	// Maintenant, on est sûr que le
	// controleur IDE est fonctionnel.
	Ctrl->State = IDE_CTRL_READY;

	// On teste les types des DEVICES.
	if (Mask & 1) IDE_InitDevice(&Ctrl->Devices[0]);
	if (Mask & 2) IDE_InitDevice(&Ctrl->Devices[1]);

	return 0;
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// Initialisation du driver IDE.
// Revoie 0 (OK) ou autre (Err).
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
char IDE_Init()
{
	int i;

	OutDbg("-- Pilote IDE (Mode PIO) lecture seule DEBUG --");

	for (i = 0; i < 2; i++)
	{
		OutDbg("Init du Controleur IDE %i.",i);
		if (IDE_InitCtrl(&Ctrls[i]) > 0)
		{
			OutDbg("Impossible d'initialiser le Controleur IDE %i.",i);
			return 1;
		}
	}

	OutDbg("Listes des DEVICES :");
#ifdef DEBUG_IDE
	for (i = 0; i < 2; i++)
	{
		int ii;
		OutDbg(" IDE %u :",i)

		for (ii = 0; ii < 2; ii++)
		{
			if (Ctrls[i].Devices[ii].State == IDE_DEVICE_NOT_PRESENT)
			{
				OutDbg("  PAS DE DEVICE");
				continue;
			}
			if (Ctrls[i].Devices[ii].State == IDE_DEVICE_ATA)
			{
				OutDbg("  ATA DISQUE DUR");
				continue;
			}
			OutDbg("  ATAPI CDROM");
		}
	}
#endif
	OutDbg("Analyse termine en %u microsecondes.",Time);

	return 0;
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// Lit sur le disque choisi un secteurs dans un buffer.
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
u8 Read(TIDE_Device* Device, u32 LBA, void* Buffer)
{
	u32 IoAddr = Device->Ctrl->IoAddr;
	u32 TimeOut;
	u8 Status;
	u32 NumSector = 1;

	OutDbg("IDE_Read LBA:%u, NumSector:%u",LBA,NumSector);
	
	u8 Lba_0 = (LBA & 0xff);
	u8 Lba_1 = (LBA >> 8) & 0xff;
	u8 Lba_2 = (LBA >> 16) & 0xff;
	u8 Lba_3 = ((LBA >> 24) & 0x7) | 0x40;

  	/*OutDbg("Reset du controleur...");
  	Outb(IoAddr + ATA_DRIVE,ATA_D_IBM | ATA_D_MASTER);
  	Timer_SleepU(1);
 	Outb(IoAddr + ATA_DEVICE_CONTROL,ATA_A_nIEN | ATA_A_RESET);
  	Timer_SleepU(1);
  	Outb(IoAddr + ATA_DEVICE_CONTROL,ATA_A_nIEN);
  	Inb(IoAddr + ATA_ERROR);*/


	OutDbg("Attend que le controlleur (%h) soit ready...",IoAddr);
	for (TimeOut = 0; TimeOut < 30; TimeOut++)
	{
		Status = Inb(IoAddr + ATA_STATUS);
		OutDbg("Essai %u / 30. Status %b1.",TimeOut+1,Status);
		if (!(Status & ATA_S_BSY)) break;
		Timer_SleepU(1);
	}

	if (TimeOut >= 30)
	{
		OutDbg("Erreur ! Timeout !");
		return 1;
	}

	/*if (Inb(IoAddr + ATA_STATUS) & ATA_S_ERROR)
	{
		OutDbg("Erreur ! Bit d'erreur actif !");
		return 1;
	}*/

	// Précision, le LBA tient sur 28 bits,
	// c'est à dire 137 go max.

	OutDbg("Envoi des paramètres de lecture...");
	Outb(IoAddr + ATA_DEVICE_CONTROL,ATA_A_nIEN | ATA_A_4BIT);
	Outb(IoAddr + ATA_ERROR,1);
	Outb(IoAddr + ATA_PRECOMP,0);
	Outb(IoAddr + ATA_SECTOR_COUNT,NumSector);
	Outb(IoAddr + ATA_LBA_0,Lba_0);
	Outb(IoAddr + ATA_LBA_1,Lba_1);
	Outb(IoAddr + ATA_LBA_2,Lba_2);
	Outb(IoAddr + ATA_LBA_3,(Device->Type == IDE_DEVICE_SLAVE?ATA_D_SLAVE:ATA_D_MASTER) | Lba_3);

	Timer_SleepU(1); // Pour l'ATA 4

	Outb(IoAddr + ATA_CMD,ATA_C_READ);

	OutDbg("Attend que le controleur soit ready.");

	for (TimeOut = 0; TimeOut < 30; TimeOut++)
	{
		Status = Inb(IoAddr + ATA_STATUS);
		OutDbg("Essai %u / 30. Status %b1.",TimeOut+1,Status);
		if (!(Status & ATA_S_BSY)) break;
		Timer_SleepU(1);
	}

	if (TimeOut >= 30)
	{
		OutDbg("Erreur ! Timeout !");
		return 1;
	}

	if (Status & ATA_S_ERROR)
	{
		OutDbg("Bit d'erreur actif !");
		return 1;
	}

	if (!(Status & ATA_S_DRQ))
	{
		OutDbg("Bit DRQ non actif !");
		return 1;
	}

	OutDbg("Lecture des données...");
	for (TimeOut = 0; TimeOut < (NumSector*256); TimeOut++)
	{
		((u16*) Buffer)[TimeOut] = Inw(IoAddr);
	}

	return 0;
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// Lit sur le disque choisi un ou plusieurs secteurs dans un buffer.
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
u8 IDE_Read(TIDE_Device* Device, u32 LBA, u8 NumSector, void* Buffer)
{
	void* ptrBuffer = Buffer;
	
	while (NumSector-- > 0)
	{
		if (Read(Device,LBA++,ptrBuffer)) return 1;
		ptrBuffer += 512;
	}
	return 0;
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// Retourne un pointeur sur la structure du Contrôleur.
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
TIDE_Controleur *IDE_GetCtrl()
{
	return Ctrls;
}
