/*
 *	Pilote Dma.
 *
 *	Bombela.
 *	08/10/2004
 *	24/10/2004
 */

//////////////////////////////////////////////
#include <Debug_Start.h>
#ifdef DEBUG_DMA
#define DEBUG_NAME "DMA     "
#endif
#include <Debug_End.h>

#include <Types.h>
#include <Io.h>
#include <OutText.h>

#include "Define.h"

const static DmaCanal ConfDma16[] =
{
   { 0x87, 0x00, 0x01 },
   { 0x83, 0x02, 0x03 },
   { 0x81, 0x04, 0x05 },
   { 0x82, 0x06, 0x07 }
};

const static DmaCanal ConfDma8[] =
{
   { 0x87, 0x00, 0x01 },
   { 0x83, 0x02, 0x03 },
   { 0x81, 0x04, 0x05 },
   { 0x82, 0x06, 0x07 }
};


char Dma_Init()
{
	OutDbg("-- Pilote du DMA (Direct Memmory Acess) DEBUG --");

	OutDbg("Configure les DMA master et slave...");
	Outb(M_Command,0);  // Mode de travail PC AT.
	Outb(S_Command,0);  // Mode de travail PC AT.

	OutDbg("Masque les canaux sauf le 0 du master (cascade).");
	Outb(M_Mask2,0x0E); // Canaux off sauf cascade.
	Outb(S_Mask2,0x0F); // Canaux off.

	return 0;
}

void Dma_SetCanal(u8 Canal,boolean Active)
{
	if (Canal < 4)
	{
		Outb(S_Mask1,Canal | (Active?0:4));
	}
	else
	{
		Outb(M_Mask1,(Canal-4) | (Active?0:4));
	}
}

// Write = false > = Read
// Canal : 0..3 > 8 bits. 5..7 > 16 bits.
// Page = 64ko en 8 bit et 128 ko en 16 bits.
// Length = u8 en 8 bits, u16 en 16 bits.
// Attention ! Aucune erreur de canal vérifiée !
char Dma_SetRequest(u8 Canal,u8 Page,u16 Length,boolean Write)
{
	boolean Dma16;
	u8 	DmaCanal;
	u16    Port_Adress;
	u16    Port_Longeur;

	if (Canal < 4)
	{
		Dma16 = false;
		DmaCanal = Canal;
	}
	else
	{
		Dma16 = true;
		DmaCanal = Canal - 4;
	}

	OutDbg("Canal %u (%s->%u), page %u, longueur %u, %s.",Canal,Dma16?"16 bits":"8 bits",DmaCanal,Page,Length,Write?"ecriture":"lecture");
	Dma_SetCanal(Canal,false); // Désactivation.

	// Flip flop.
	Outb((Dma16?M_FlipFlop:S_FlipFlop),0);

	// Le mode...
	Outb((Dma16?M_Mode:S_Mode),(Write?0x48:0x44) + DmaCanal);

	Outb(Dma16?ConfDma16[DmaCanal].Page:ConfDma8[DmaCanal].Page,Page);

	// Envoi offset. (0 pour début de page).
	Port_Adress = Dma16?ConfDma16[DmaCanal].Adresse:ConfDma8[DmaCanal].Adresse;
	Outb(Port_Adress,0);
	Outb(Port_Adress,0);

	// Envoi de la longueur du transfert.
	Port_Longeur = Dma16?ConfDma16[DmaCanal].Long:ConfDma8[DmaCanal].Long;
	Outb(Port_Longeur,--Length);
	Outb(Port_Longeur,(Length >> 8));

	Dma_SetCanal(Canal,true); // Et on active !

	return 0;
}
