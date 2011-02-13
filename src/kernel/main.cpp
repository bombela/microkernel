/*
 * main.cpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <cxxruntime.h>

#include <kernel/multiboot.h>
#include <kernel/console.h>
#include <kernel/die.h>
#include <cxxruntime.h>
#include <kernel/interrupt.h>
#include <kernel/pic_i8259.h>
#include <kernel/exception.h>
#include <iostream>
#include <iomanip>
#include <bind>

namespace kernel {

// multiboot specification header
SECTION(".multiboot") ALIGNED(4) multiboot::header_short
	multiboot_header(MULTIBOOT_PAGE_ALIGN bitor MULTIBOOT_MEMORY_INFO);

} // namespace kernel

void divide_error(UNUSED int nb)
{
	using namespace ::std;
	cout << color::red << "divide error" << color::ltgray << endl;
	::kernel::die();
}

extern "C" void kernel_main(UNUSED int magic, UNUSED void* multiboot_addr)
{
	{
		using namespace kernel;

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
	}

	cxxruntime::Run running;
	
	std::cout("Kernel %running%...",
			std::color::green, std::color::ltgray) << std::endl;

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		std::cout("%Not loaded by a multiboot compliant loader\n", std::color::red);
		kernel::die();
	}

	//kernel::InterruptManager interrupManager;

	/*const int max = 3;
	for (int i = 1; i <= max; ++i)
	{
		std::cout << "Waiting wakeup..."
			<< std::format(" (%//%)", i, max) << std::endl;
		asm ("hlt");
	}*/
}
