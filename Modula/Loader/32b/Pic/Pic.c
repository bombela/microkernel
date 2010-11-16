/*
 *	Pilote Pic.
 *
 *	Bombela.
 *	03/07/2004
 *	02/10/2004
 * 	11/26/2005 - Ajout de Pic_Release
 */

//////////////////////////////////////////////
#include <Debug_Start.h>
#ifdef DEBUG_PIC
#define DEBUG_NAME "PIC     "
#endif
#include <Debug_End.h>

#include <Types.h>
#include <Io.h>
#include <OutText.h>

#include "Define.h"

char Pic_Init()
{
	OutDbg("-- Pilote du PIC (Programmable Interrupt Controleur) DEBUG --");

	#define ICW1_M	0x11 // Envoi de ICW4_M et mode 2 PIC.
	#define ICW2_M	  32 // Int de base.
	#define ICW3_M	   4 // IRQ 2 = PIC slave.
	#define ICW4_M	   1 // Mode Intel, EOI manuel.
	#define	OCW1_M	0xFB // Toutes les INT masquée sauv celle du slave !

	#define ICW1_S	0x11 // Envoi de ICW4_M et mode 2 PIC.
	#define ICW2_S	  40 // Int de base.
	#define ICW3_S	   2 // Relier au maître par l'int 2.
	#define ICW4_S	   1 // Mode Intel, EOI manuel.
	#define	OCW1_S	0xFF // Toutes les INT masquée !

	OutDbg("Initialisation du master PIC, seul IRQ du slave PIC activée...");

    Outb(ICW001_M, ICW1_M);
    Outb(ICW234_M, ICW2_M);
    Outb(ICW234_M, ICW3_M);
    Outb(ICW234_M, ICW4_M);
    Outb(OCW001_M, OCW1_M);

	OutDbg("Initialisation du slave PIC, toutes les IRQ masquées...");

    Outb(ICW001_S, ICW1_S);
	Outb(ICW234_S, ICW2_S);
	Outb(ICW234_S, ICW3_S);
	Outb(ICW234_S, ICW4_S);
    Outb(OCW001_S, OCW1_S);

	return 0;
}


char Pic_Release()
{
	OutDbg("-- Release --");

	OutDbg("Toutes les IRQ du slave PIC masquées...");
	Outb(OCW001_S, OCW1_S);	
	
	OutDbg("Toutes les IRQ master PIC masquées...");
	Outb(OCW001_M, OCW1_S);
	
	return 0;
}

static u8 NbToBit(u8 Nb)
{
	u8 i;
	u8 r = 1;
	for (i = 0; i < Nb; i++) r <<= 1;
	return r;
}

// Active/Désactive une IRQ.
// Erreur : 1 = IRQ invalide.
char Pic_IrqMask(u8 NumIrq, boolean Active)
{
	u8 Mask;

	if ((NumIrq > 1) && (NumIrq < 10))
	{
		// Slave PIC
		OutDbg("IRQ%u -> Slave PIC -> Ligne %u.",NumIrq,NumIrq-2);
		NumIrq = NbToBit(NumIrq-2);

		Mask = Inb(OCW001_S);
		if (Active)
		{
			Outb(OCW001_S, Mask & ~NumIrq);
			OutDbg("Activation...");
		}
		else
		{
			Outb(OCW001_S, Mask | NumIrq);
			OutDbg("Inactivation...");
		}
		OutDbg("Action réussie.");
		return 0;
	}
	if (NumIrq < 15)
	{
		// Master PIC
		if (NumIrq > 9)
		{
			OutDbg("IRQ%u -> Master PIC -> Ligne %u.",NumIrq,NumIrq-8);
			NumIrq = NbToBit(NumIrq-7);
		}
		else
		{
			OutDbg("IRQ%u -> Master PIC -> Ligne %u.",NumIrq,NumIrq);
			NumIrq = NbToBit(NumIrq);
		}

		Mask = Inb(OCW001_M);
		if (Active)
		{
			OutDbg("Activation...");
			Outb(OCW001_M, Mask & ~NumIrq);
		}
		else
		{
			OutDbg("Inactivation...");
			Outb(OCW001_M, Mask | NumIrq);
		}
		OutDbg("Action réussie.");
		return 0;
	}
	OutDbg("IRQ Inexistante !");
	return 1;
}

// Donne l'int correspondant à l'IRQ.
// Si bad irq, renvoie 0.
char Pic_IrqToInt(u8 Irq)
{
	if ((Irq > 1) && (Irq < 10))
	{
		//Slave
		return ICW2_S+Irq-2;
	}
	if (Irq < 15)
	{
		//Master
		if (Irq > 9) return ICW2_M+Irq-7; else return ICW2_M+Irq;
	}
	OutDbg("Pic_IrqToInt > IRQ Invalide : %u.",Irq);
	return 0;
}

// Termine une irq sur le PIC ou les PIC.
// Retourne 0 si OK.
// retourne 1 si IRQ inexistante.
char Pic_EndIrq(u8 Irq)
{
	if ((Irq > 1) && (Irq < 10))
	{
		//Slave
		Outb(ICW001_S,0x20); // Fin pic slave.
		Outb(ICW001_M,0x20); // Fin pic master.
		return 0;
	}
	if (Irq < 15)
	{
		//Master
		Outb(0x20,0x20); // Fin pic master.
		return 0;
	}
	OutDbg("Pic_EndIrq > IRQ Invalide !");
	return 0;
}
