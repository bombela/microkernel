/*
 * main.cpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <cxxruntime.h>

#include <kernel/multiboot.h>
#include <kernel/console.h>
#include <kernel/std/array.hpp>
#include <kernel/die.h>

namespace kernel {

// multiboot specification header
SECTION(".multiboot") ALIGNED(4) multiboot::header_short
	multiboot_header(MULTIBOOT_PAGE_ALIGN bitor MULTIBOOT_MEMORY_INFO);

} // namespace kernel

#if 0
kernel::std::console& mainconsole = kernel::std::console::getInstance();

struct Toto
{
	Toto() { mainconsole.write("constructor\n"); }
	~Toto() { mainconsole.write("destructor\n"); }
};

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
		mainconsole.write("constructor style ");
		mainconsole.write(name);
		mainconsole.write("\n");
	}

	Titi(const char* name):
		_name(name)
	{
		mainconsole.write("constructor ");
		mainconsole.write(name);
		mainconsole.write(" --\n");
	}

	~Titi() {
		mainconsole.write("destructor ");
		mainconsole.write(_name);
		mainconsole.write(" --\n");
	}
	T var;
};

//Titi<int, 10> a("int");
//Titi<char, 12> b("char");

//Titi< Titi<float, 14>, 16> c("Titi<float>", "float");

#endif

extern "C" void kernel_main(int magic, void* multiboot_addr)
{
	kernel::main_console_init();

	kernel::main_console->write("Hi everybody, welcome to the kernel wrote in ");
	kernel::main_console->setAttr({
			kernel::Console::Color::ltgray,
			kernel::Console::Color::blue
			});
	kernel::main_console->write("C++0x");
	kernel::main_console->setAttr({
			kernel::Console::Color::ltmagenta,
			kernel::Console::Color::black
			});
	kernel::main_console->write(" :)");
	kernel::main_console->resetAttr();
	kernel::main_console->write("\n");
}
