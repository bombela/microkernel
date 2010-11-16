/*
 * Screen Saver.
 *
 * System support : Modula.
 *
 */

//////////////////////////////////////////////

#include <Types.h>
#include <Arg.h>
#include <Io.h>
#include <OutText.h>
#include <Int.h>
#include <Pic.h>
#include <Timer.h>
#include <Dma.h>
#include <Floppy.h>
#include <Fat12.h>


void ScrSaver()
{
	OutT_Init();

	Int_Init();
	Pic_Init();
	Timer_Init();

	OutT_SetAttr(13);
	OutT("\nBon ok, c'est stupid de linker ça statiquement\nMais pour la beauté du test... :D\n");

	OutT_SetAttr(11+128);
	OutT("\n\n							Bombela.\n");

	OutT_Release();
}

