/*
 *	Petit pilote IDE assurant seulement la lecture.
 *  Déclarations.
 *
 *	Bombela.
 *	30/06/2004
 *	29/09/2004
 */

#pragma pack (1)

typedef struct
{
	enum { IDE_DEVICE_MASTER, IDE_DEVICE_SLAVE } Type;
	enum { IDE_DEVICE_NOT_PRESENT, IDE_DEVICE_ATA, IDE_DEVICE_ATAPI } State;
	struct IDE_Controleur* Ctrl;
} TIDE_Device;

typedef struct IDE_Controleur
{
	u16 IoAddr;
	enum { IDE_CTRL_NOT_PRESENT, IDE_CTRL_READY } State;

	TIDE_Device Devices[2];

} TIDE_Controleur;

#pragma pack ()

// Fonctions.

char IDE_Init();
TIDE_Controleur *IDE_GetCtrl();
u8 IDE_Read(TIDE_Device* Device, u32 LBA, u8 NumSector, void* Buffer);
