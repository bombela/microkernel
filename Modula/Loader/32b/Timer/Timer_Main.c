/*
 *	Pilote timer.
 *
 *	Bombela.
 *	03/07/2004
 *	24/12/2004
 *	29/01/2006 - Externalisation de la fonction de calucl de vitesse
 *			du CPU ainsi qu'ajout d'une fonction permettant
 *			de récupérer la valeur précédament stoquée...
 */

//////////////////////////////////////////////

#include <Debug_Start.h>
#ifdef DEBUG_TIMER
#define DEBUG_NAME "TIMER   "
#endif
#include <Debug_End.h>

#include <Types.h>
#include <Io.h>
#include <OutText.h>
#include <Int.h>
#include <Pic.h>

#include "_Timer.h"
#include "Timer.h"

// Fonctions dans fichiers asm
//extern void INT_Timer();
extern void CPU_Speed(struct RDTSC2 *PtrRDTSC2);

// Vitesse CPU.
volatile u32 Timer_Counter; // Le compteur de la procédure de timer.
volatile u8 Timer_On; // L'indicateur d'activation du timer.
u32 CpuSpeed;

void SetCount(u16 Count)
{

	// Diviser 1 193 180 par Count donne le nombre de
	// "click" par secondes. (Attention, 0 représente 65536).
	// Si je met 1, ça signifie ~ 1 microseconde,
	// 838 nanosecondes plus précisément...

	// Envoie le Count au compteur 0...
	Outb(0x43,36); // Initialise le compteur 0 avec le mode 2.
	Outb(0x40,Count);
	Outb(0x40,Count >> 8);
}

extern void* TimerIntAsm();

void TimerInt()
{
	if (Timer_On)
	{
		Timer_Counter--;
		if (Timer_Counter == 0) Timer_On = 0;
	}
}

// Initialisation du pilote timer.
// Revoie 0 (OK) ou 1 (ERR).
char Timer_Init()
{
	OutDbg("-- Pilote du timer 8253 compilation DEBUG --");

	OutDbg("Connexion de l'IRQ0 (Timer) à la fonction INT du timer...");
	Int_SetInt(Pic_IrqToInt(0),TimerIntAsm);

	OutDbg("Initialisation du compteur 0 en mode 2...");
	SetCount(0);

	OutDbg("Activation de l'IRQ0...");
	Pic_IrqMask(0, true);

	Timer_CalcCpuSpeed();

	return 0;
}

void Timer_SleepU(u32 MicroSecondes)
{
	asm(" \n\t \
		mull (CpuSpeed) \n\t \
		movl %%eax, %%ecx \n\t \
		Boucle: \n\t \
		loopl Boucle \n\t \
	"
	:
	:"eax" (MicroSecondes)
	:"ecx", "edx"
	);
}

void Timer_SleepM(u32 MilliSecondes)
{
	Timer_Counter = MilliSecondes;

	SetCount(1193); // Interval 1 ms.

	Timer_On = 1;
	while (Timer_On) __asm__ ("hlt");

	SetCount(0); // Interval 55 ms.
}

void Timer_SleepM55(u32 Multiple)
{
	Timer_Counter = Multiple;

	SetCount(0); // 55 ms.
	Timer_On = 1;
	while (Timer_On) __asm__ ("hlt");
}

u32 Timer_CalcCpuSpeed()
{
	struct RDTSC2 USE_RDTSC2;
	u32 Speed;

	OutDbg("Calcul de la vitesse du CPU...");
	
	do
	{
		CPU_Speed(&USE_RDTSC2);

		Speed = USE_RDTSC2.RDTSC_2 - USE_RDTSC2.RDTSC_1;
	} while (Speed <= 0);


	CpuSpeed = Speed / 54925; // rapport à 55 ms d'attente dans CPU_Speed.

	// Dans le cas où le CPU = 0 Mhz (Sous bochs par exemple)
	// on rectifie à 1 Mhz.
	if (CpuSpeed < 1) CpuSpeed = 1;
	
	OutDbg("CPU %u Mhz.",CpuSpeed);
	
	return CpuSpeed; 
}

u32 Timer_GetCpuSpeed()
{
	return CpuSpeed;
}
