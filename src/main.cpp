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
#include <iomanip>

namespace kernel {

// multiboot specification header
SECTION(".multiboot") ALIGNED(4) multiboot::header_short
	multiboot_header(MULTIBOOT_PAGE_ALIGN bitor MULTIBOOT_MEMORY_INFO);

} // namespace kernel

using namespace kernel;

extern "C" void kernel_main(int magic, void* multiboot_addr)
{
	main_console_init();

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

	cxxruntime::Run running;
	
	//kernel::Interrupt interrupt;

	// make a "double fault exception"
	//while (true)
	//	asm ("hlt");

	//make a "divide by zero exception"
	//int i = 42 / 0;

	using namespace kernel::std;
	cout << "cout is fonctionnal! "
		<< color::blue << "ahah it's work" << color::ltgray << endl;
}
