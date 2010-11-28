/*
 * main.cpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

extern "C" void _start()
{
	// inject asm here, setup a stack etc.

	// stop cpu.
	asm("\
			cli \n\
			hlt \n\
			");

	// should never reach here.
	while (1) ;
}
