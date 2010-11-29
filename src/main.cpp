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
			char* vmem = (char*) VideoMem + (2 * 80) * (line); \
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
func_ptr cleanup[20] = { 0 };

void* cleanup_self[20];

extern "C" int __cxa_atexit(void (*func)(), void* self)
{
	msg("__cxa_atexit", 6);
	
	typedef void (*func_ptr)();
	func_ptr* fend = cleanup;
	int i = 0;
	while (*fend)
	{
		++fend;
		++i;
	}
	*fend = func;
	*++fend = 0;
	cleanup_self[i] = self;
	return 0;
}

Toto toto;

template <typename T, int L>
struct Titi
{
	const char* _name;

	template <typename U>
	Titi(const char* name, U arg1):
		_name(name),
		var(arg1)
	{
		msg("                                            constructor style", L);
		msg(name, L);
	}

	Titi(const char* name):
		_name(name)
	{
		msg("                                            constructor", L);
		msg(name, L);
	}

	~Titi() {
		msg("                                            destructor", L + 1);
		msg(_name, L + 1);
	}
	T var;
};

Titi<int, 10> a("int");
Titi<char, 12> b("char");

Titi< Titi<float, 14>, 16> c("Titi<float>", "float");

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

		for (func_ptr* i = __e_ctors - 1; i >= __b_ctors; --i)
			(*i)();
	}

	{
		msg("call destructor", 24);
		func_ptr* i = cleanup;
		int j = 0;
		while (*i)
		{
			++i;
			++j;
		}
		while (--j, --i >= cleanup)
			((void (*)(void*))(*i)) (cleanup_self[j]);
	}
}
