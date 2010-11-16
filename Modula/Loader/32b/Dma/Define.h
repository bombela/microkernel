/*
 *	Pilote DMA.
 *
 *	Bombela.
 *	08/10/2004
 *	24/10/2004
 */

typedef struct DmaCanal
{
   u8 Page;
   u8 Adresse;
   u8 Long;
} DmaCanal;

// Registres Master.
#define M_Statut	0xD0 // R
#define M_Command	0xD0 // W
#define M_Request	0xD2 // W
#define M_Mask1		0xD4 // W
#define M_Mode		0xD6 // W
#define M_FlipFlop	0xD8 // W
#define M_TempMem	0xDA // R
#define M_Reset		0xDA // W
#define M_MaskReset	0xDC // W
#define M_Mask2		0xDE // W

#define S_Canal_Adresse		0xC0 // W | + 4 pour les canaux suivants.
#define S_Canal_Longueur	0xC2 // W | + 4 pour les canaux suivants.

// Registres Slave.
#define S_Statut	0x08 // R
#define S_Command	0x08 // W
#define S_Request	0x09 // W
#define S_Mask1		0x0A // W
#define S_Mode		0x0B // W
#define S_FlipFlop	0x0C // W
#define S_TempMem	0x0D // R
#define S_Reset		0x0D // W
#define S_MaskReset	0x0E // W
#define S_Mask2		0x0F // W

#define M_Canal_Adresse		0x00 // W | + 2 pour les canaux suivants.
#define M_Canal_Longueur	0x01 // W | + 2 pour les canaux suivants.
