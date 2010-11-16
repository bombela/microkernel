/*
 *	OutText, déclaration de OutText.elf.
 *
 *	Bombela.
 *	28/06/2004
 *	22/04/2005
 */

#include <Types.h>

void OutT_Init();
void OutT_Release();

void OutT_SetAttr(unsigned char NewAttribut);
unsigned char OutT_GetAttr();
void OutT_PutChar(unsigned char Char);
void OutT_Format(unsigned char *str, void *params);

// %u  : unsigned int.
// %i  : int.
// %U? : unsigned int. ? = Le nombre de zero.
// %I? : int.		   ? = Le nombre de zero.
// %s  : ptrString.
// %h  : hexa.
// %b? : binaire. ? = 1 pour un octets, 2 pour deux octets etc. Max = 4.
// %c  : char.
// *params est la liste des paramètres %?.
void OutT(unsigned char *str, ...);

u8 OutT_LogStart();
void OutT_LogStop();
