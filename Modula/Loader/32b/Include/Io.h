/*
 *  Macro I/O
 *
 *	Bombela.
 *	03/03/2005
 *	28/01/2006 - Prise d'exemple sur les includes de linux, ainsi
			qu'une petite lecture de doc asm inline gcc ;)
 */

#ifndef include_io
#define include_io

#include <Types.h>

static __inline void Outb(Port,Data)
{
	__asm__ __volatile__ ("outb %b0, %w1"::"a"(Data),"Nd"(Port));
}

static __inline void Outw(Port,Data)
{
	__asm__ __volatile__ ("outw %w0, %w1"::"a"(Data),"Nd"(Port));
}

static __inline u8 Inb(Port)
{
	u8 Ret;
	__asm__ __volatile__ ("inb %w1, %0":"=a"(Ret):"Nd"(Port));
	return Ret;
}

static __inline u16 Inw(Port)
{
	u16 Ret;
	__asm__ __volatile__ ("inw %w1, %0":"=a"(Ret):"Nd"(Port));
	return Ret;
}

#endif

