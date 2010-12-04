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
	
}
