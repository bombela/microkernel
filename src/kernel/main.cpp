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
#include <kernel/segmentation.h>
#include <kernel/interrupt.h>
#include <iostream>
#include <iomanip>
#include <bind>

namespace kernel {

// multiboot specification header
SECTION(".multiboot") ALIGNED(4) multiboot::header_short
	multiboot_header(MULTIBOOT_PAGE_ALIGN bitor MULTIBOOT_MEMORY_INFO);

// essential harware management

segmentation::Manager segmentationManager;
interrupt::Manager    interruptManager;

} // namespace kernel

extern "C" void kernel_main(UNUSED int magic,
		UNUSED const multiboot::info* const mbi)
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
		std::cout("%Not loaded by a multiboot compliant loader%\n",
				std::color::red, std::color::ltgray);
		kernel::die();
	}

	std::cout("Mem = %MB (upper limit = %xkB)\n",
			(mbi->mem_upper >> 10), mbi->mem_upper);

	struct APICVersionRegister {
		uint8_t version;
		uint8_t reserved1;
		uint8_t maxlvtentry;
		uint8_t supportsupressEOIbroadcast:1;
		uint8_t reserved2:7;
	} PACKED volatile const * const avr
	= reinterpret_cast<APICVersionRegister*>(0xFEE00030);

	std::cout("APIC version=%c maxlvtentry=%c\n",
			avr->version, avr->maxlvtentry);

	const int max = 3;
	for (int i = 1; i <= max; ++i)
	{
		int a = 1 / 0;
		std::cout << "Waiting wakeup..."
			<< std::format(" (%//%)", i, max) << std::endl;
		asm ("hlt");
	}

	std::cout("kernel stopping...\n");
}
