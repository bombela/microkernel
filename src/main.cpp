/*
 * main.cpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <kernel/multiboot.h>
#include <kernel/console.h>

#include <kernel/std/array.hpp>
#include <kernel/die.h>

namespace kernel {

// multiboot specification header
SECTION(".multiboot") ALIGNED(4) multiboot::header_short
	multiboot_header(MULTIBOOT_PAGE_ALIGN bitor MULTIBOOT_MEMORY_INFO);

} // namespace kernel

kernel::std::console console;

struct Toto
{
	Toto() { console.write("constructor\n"); }
	~Toto() { console.write("destructor\n"); }
};

void* __dso_handle = 0;

typedef void (*func_ptr)();
func_ptr cleanup[20] = { 0 };

void* cleanup_self[20];

extern "C" int __cxa_atexit(void (*func)(), void* self)
{
	console.write("__cxa_atexit\n");
  
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
		console.write("constructor style ");
		console.write(name);
		console.write("\n");
	}

	Titi(const char* name):
		_name(name)
	{
		console.write("constructor ");
		console.write(name);
		console.write(" --\n");
	}

	~Titi() {
		console.write("destructor ");
		console.write(_name);
		console.write(" --\n");
	}
	T var;
};

Titi<int, 10> a("int");
Titi<char, 12> b("char");

Titi< Titi<float, 14>, 16> c("Titi<float>", "float");

extern "C" void kernel_main(int magic, void* multiboot_addr)
{
	{
		typedef void (*func_ptr)();
		extern func_ptr __b_ctors[];
		extern func_ptr __e_ctors[];

		//console.write("call constructors (never displayed)\n");
		for (func_ptr* i = __e_ctors - 1; i >= __b_ctors; --i)
			(*i)();
	}

	for (int i=0; i< 5;++i) {
	  console.setColor((kernel::std::console_color)i);
	  console.write("coucou\n");
	}
	console.setColor(kernel::std::realwhite);

	/*for (int i = 0; i < 1000; ++i)
	{
		console.write("A");
		console.write("B");
		console.write("c");
	}*/

	{
		console.write("call destructor\n");
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
