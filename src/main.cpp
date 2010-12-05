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
#include <cxxruntime.h>
#include <kernel/interrupt.h>
#include <iostream>

namespace kernel {

// multiboot specification header
SECTION(".multiboot") ALIGNED(4) multiboot::header_short
	multiboot_header(MULTIBOOT_PAGE_ALIGN bitor MULTIBOOT_MEMORY_INFO);

} // namespace kernel

using namespace kernel;

struct Toto
{
	Toto() { main_console->write("constructor toto\n"); }
	~Toto() { main_console->write("destructor toto\n"); }
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
		main_console->write("constructor style ");
		main_console->write(name);
		main_console->write("\n");
	}

	Titi(const char* name):
		_name(name)
	{
		main_console->write("constructor ");
		main_console->write(name);
		main_console->write(" --\n");
	}

	~Titi() {
		main_console->write("destructor ");
		main_console->write(_name);
		main_console->write(" --\n");
	}
	T var;
};

Titi<int, 10> a("int");
Titi<char, 12> b("char");

Titi< Titi<float, 14>, 16> c("Titi<float>", "float");
Titi<char, 12> d("char");
Titi<char, 12> e("char");
Titi<char, 12> f("char");

extern "C" void kernel_main(int magic, void* multiboot_addr)
{
	main_console_init();
	cxxruntime::Run running;

	//kernel::Interrupt interrupt;

	// make a "double fault exception"
	//while (true)
	//	asm ("hlt");

	//make a "divide by zero exception"
	//int i = 42 / 0;

	main_console->write("Hi everybody, welcome to the kernel wrote in ");
	main_console->setAttr({
			Console::Color::ltgray,
			Console::Color::blue
			});
	main_console->write("C++0x");
	main_console->setAttr({
			Console::Color::ltmagenta,
			Console::Color::black
			});
	main_console->write(" :)");
	main_console->resetAttr();
	main_console->write("\n");

	using namespace kernel;
	kernel::std::cout << "cout is fonctionnal!" << kernel::std::endl;
}
