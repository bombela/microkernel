/*
 *  Macro I/O
 *
 *	Bombela.
 *	03/03/2005
 *	03/03/2005
 */

#ifndef include_io
#define include_io


#define Outb(PORT,DATA) asm("outb %%al, %%dx"::"dx"(PORT),"al"(DATA))
#define Outw(PORT,DATA) asm("outb %%al, %%dx"::"dx"(PORT),"ax"(DATA))


#define Inb(PORT) \
({ \
  unsigned char ret; \
	asm("inb %%dx, %%al":"=al"(ret):"dx"(PORT)); \
	ret; \
})

#define Inw(PORT) \
({ \
	unsigned short int ret; \
	asm("inw %%dx, %%ax":"=ax"(ret):"dx"(PORT)); \
	ret; \
})

#endif

