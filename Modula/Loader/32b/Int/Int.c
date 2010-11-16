/*
 *	Pilote Int.
 *
 *	Bombela.
 *	03/07/2004
 *	01/10/2004
 */

#define IDT_Size 47

//////////////////////////////////////////////

#include <Debug_Start.h>
#ifdef DEBUG_INT
#define DEBUG_NAME "INT     "
#endif
#include <Debug_End.h>

#include <Types.h>
#include <OutText.h>

#include "Structs.h"


// Permet de définir une procédure d'exception :
// void ExceptionNUM() qui affiche du texte et
// bloque le PC.
#define Define_Except(NUM, TEXT) void Exception ## NUM () \
					{ \
						OutT_SetAttr(12); \
						OutT("\nException [%u] : "TEXT".",NUM); \
						while (true); \
					}

Define_Except(0,"Division par 0")
Define_Except(1,"Pas à pas")
Define_Except(2,"NMI, Erreur mémoire")
Define_Except(3,"Point d'arret.")
Define_Except(4,"Depassement de TRAP")
Define_Except(5,"Depassement avec BOUND")
Define_Except(6,"Instruction CPU inconnue")
Define_Except(7,"Pas de coprocesseur")
Define_Except(8,"Double faute")
Define_Except(9,"Depassement de segment du coprocesseur")
Define_Except(10,"TSS non valide")
Define_Except(11,"Segment indisponible")
Define_Except(12,"Erreur de pile")
Define_Except(13,"Acces de segment non autorise")
Define_Except(14,"Faute de page")
Define_Except(15,"Exception reservee. Inconue...")
Define_Except(16,"Erreur du coprocesseur")

// L'IDT en elle même...
IDT_Item IDT[IDT_Size];

// Fait pointer les exceptions (0..16) vers leurs routines.

#define IDT_ElemDefine(INDEX,OFFSET) \
	{ \
		unsigned int Offset = (unsigned int) OFFSET; \
		IDT[INDEX].Selecteur = 8; \
		IDT[INDEX].Type = 0x8E00; \
		IDT[INDEX].Offset_Low  = Offset; \
		IDT[INDEX].Offset_High = Offset >> 16; \
	}

#define IDT_ExceptDefine(NUM) IDT_ElemDefine(NUM,Exception ## NUM);

// -------------------------------------------

char Int_Init()
{
	// La petite structure pour le LIDT.
	IDT_ptrIDTR ptrIDT;

	OutDbg("-- Pilote des interruptions. Compilation DEBUG --");

	OutDbg("Connexion des procédures d'exceptions dans l'IDT...");
	IDT_ExceptDefine(0)
	IDT_ExceptDefine(1)
	IDT_ExceptDefine(2)
	IDT_ExceptDefine(3)
	IDT_ExceptDefine(4)
	IDT_ExceptDefine(5)
	IDT_ExceptDefine(6)
	IDT_ExceptDefine(7)
	IDT_ExceptDefine(8)
	IDT_ExceptDefine(9)
	IDT_ExceptDefine(10)
	IDT_ExceptDefine(11)
	IDT_ExceptDefine(12)
	IDT_ExceptDefine(13)
	IDT_ExceptDefine(14)
	IDT_ExceptDefine(15)
	IDT_ExceptDefine(16)

	OutDbg("Préparation de la structure pour LIDT...");
	ptrIDT.Base = IDT;
	ptrIDT.Limite = IDT_Size * 8; // La taille en octets de l'IDT.

	// Chargement de l'IDT.
	OutDbg("Chargement de l'IDT.");
	asm("lidtl %0"::"m"(ptrIDT));

	OutDbg("Activation des interruptions.");
	asm("sti");

	return 0;
}

// Hop on désactive les interruptions.
char Int_Release()
{	
	OutDbg("-- Release --");
	OutDbg("Désactivation des interruptions.");
	asm("cli");
	
	return 0;
}

// Permet de donner la fonction que devra
// appelé une interruption.
// Ne vérifie pas l'existence d'une int !
// Erreur : 1 = Int déjà prise.
char Int_SetInt(u16 Int, void* Offset)
{
	OutDbg("Allocage de l'INT%u pour l'offset %h...",Int,(u32) Offset);
	if (IDT[Int].Type == 0x8E00)
	{
		OutDbg("Int déjà allouée.");
		return 1;
	}
	IDT[Int].Selecteur = 8;
	IDT[Int].Type = 0x8E00;
	IDT[Int].Offset_Low  = (u32) Offset;
	IDT[Int].Offset_High = (((u32) Offset) >> 16);
	OutDbg("Allocation réussie de l'int %u à l'offset %h.",Int,(u32) Offset);
	return 0;
}
