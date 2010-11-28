/*
 * main.cpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <multiboot.h>

namespace kernel {

const uint32_t STACK_SIZE = 0x4000;

// multiboot specification header
SECTION(".multiboot") ALIGNED(4) multiboot::header_short
	multiboot_header(MULTIBOOT_PAGE_ALIGN bitor MULTIBOOT_MEMORY_INFO);

} // namespace kernel

extern "C" void kernel_main(int magic, void* multiboot_addr)
{
	#define VideoMem	0xB8000

	for (int i = 0; i < 200; i++)
	{
		char* vmem = (char*) VideoMem + (2 * 80) * 3;
		const char* hello = "Bonjour le monde...";
		while (*hello)
		{
			*vmem++ = *hello++;
			*vmem++ = 5;
		}
	}
}
