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
SECTION(".multiboot") ALIGNED(8) multiboot::header_short multiboot_header = {
	MULTIBOOT_HEADER_MAGIC,
	MULTIBOOT_HEADER_FLAGS,
	-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
};

} // namespace kernel

extern "C" void kernel_main(int magic, void* multiboot_addr)
{
	// inject asm here, setup a stack etc.
	
	#define VideoMem	0xB8000

	for (int i = 0; i < 200; i++)
	{
		char* vmem = (char*) VideoMem;
		vmem[0] = 'l';
		vmem[1] = 'o';
		vmem[2] = 'l';
		vmem[3] = 'o';
	}
}
