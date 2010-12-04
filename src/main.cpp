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

kernel::std::console& console = kernel::std::console::getInstance();

struct Toto
{
	Toto() { console.write("constructor\n"); }
	~Toto() { console.write("destructor\n"); }
};

// support for static inside function

// TODO
// it's a 64bits int, we can do what we want with it.
typedef uint64_t __guard;

// TODO implement fully
/*extern "C" void __cxa_guard_acquire(__guard*)
{
	// acquire mutex
}*/

// TODO implement fully
/*extern "C" void __cxa_guard_release(__guard*)
{
	// release mutex
}*/

// support for placement new
inline void* operator new(size_t, void* p) throw() { return p; }
inline void* operator new[](size_t, void* p) throw() { return p; }

// support for global initalisatoin
//void* __dso_handle = 0;

typedef void (*func_ptr)();
func_ptr cleanup[20] = { 0 };

void* cleanup_self[20];

/*extern "C" int __cxa_atexit(void (*func)(), void* self)
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
}*/

//Toto toto;

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

//Titi<int, 10> a("int");
//Titi<char, 12> b("char");

//Titi< Titi<float, 14>, 16> c("Titi<float>", "float");

extern "C" void kernel_main(int magic, void* multiboot_addr)
{
	kernel::std::console::initInstance();
	kernel::std::console& console = kernel::std::console::getInstance();

	console.write("welcome dude\n");
	{
		typedef void (*func_ptr)();
		extern func_ptr __b_ctors[];
		extern func_ptr __e_ctors[];

		console.write("call constructors\n");
		// will also initialize the global console& ref.
		for (func_ptr* i = __e_ctors - 1; i >= __b_ctors; --i)
			(*i)();
	}

	console.write("coucou\n");
	// for (int i=0; i< 5;++i) {
	//   console.setColor((kernel::std::color)i);
	//   console.write("coucou\n");
	// }
	// console.setColor(kernel::std::color::white);

	for (int i = 0; i < 10; ++i)
	{
		console.write("A");
		console.write("B");
		console.write("c");
	}

	console.write("\n");

	console.write("test\ntiti\ttoto\ntutu\rtata\n");

	// //kernel::die();
	
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
