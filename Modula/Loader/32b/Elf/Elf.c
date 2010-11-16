// Gestion des ELF.

#include <Debug_Start.h>
#ifdef DEBUG_ELF
#define DEBUG_NAME "ELF     "
#endif
#include <Debug_End.h>

#include <OutText.h>
#include <Elf.h>

#include "linux_elf.h"


u8 Elf_Init()
{
	OutDbg("Initialisation...");
	
	OutDbg("C'est vide là :o");
	
	return 0;
}
