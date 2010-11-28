/*
 * main.cpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <multiboot.h>

namespace kernel {

const uint32_t STACK_SIZE = 0x4000;

const uint32_t MULTIBOOT_HEADER_FLAGS =
	(MULTIBOOT_PAGE_ALIGN bitor MULTIBOOT_MEMORY_INFO);

// multiboot specification header
/*ALIGNED(8) multiboot::short_header multiboot_header = {
	MULTIBOOT_HEADER_MAGIC,
	MULTIBOOT_HEADER_FLAGS,
	-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
}; */

} // namespace kernel

extern "C" void _startlol()
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
