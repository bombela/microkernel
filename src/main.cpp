/*
 * main.cpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <multiboot.h>

#define VideoMem	0xB8000

namespace kernel {

// multiboot specification header
SECTION(".multiboot") ALIGNED(4) multiboot::header_short
	multiboot_header(MULTIBOOT_PAGE_ALIGN bitor MULTIBOOT_MEMORY_INFO);

} // namespace kernel

#define msg(msg, line) \
	do { \
		for (int i = 0; i < 200; i++) \
		{ \
			char* vmem = (char*) VideoMem + (2 * 80) * line; \
			const char* hello = msg; \
			while (*hello) \
			{ \
				*vmem++ = *hello++; \
				*vmem++ = 5; \
			} \
		} \
	} while(0)

struct Toto
{
	Toto() { msg("constructor", 4); }
	~Toto() { msg("destructor", 5); }
};

void* __dso_handle = 0;

typedef void (*func_ptr)();
func_ptr cleanup[10] = { 0 };

extern "C" void __cxa_atexit(void (*func)())
{
	msg("__cxa_atexit", 6);
	
	typedef void (*func_ptr)();
	func_ptr* fend = cleanup;
	while (*fend) ++fend;
	*fend = func;
	*++fend = 0;
}

Toto toto;

extern "C" void kernel_main(int magic, void* multiboot_addr)
{
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

	{
		msg("call constructors", 23);
		typedef void (*func_ptr)();
		extern func_ptr __b_ctors[];
		extern func_ptr __e_ctors[];

		for (func_ptr* i = __e_ctors - 1; i > __b_ctors; --i)
			(*i)();
	}

	{
		msg("call destructor", 24);
		for (func_ptr* i = cleanup; *i; ++i)
			(*i)();
	}
}
