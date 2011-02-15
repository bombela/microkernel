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

NOINLINE void enableFPU() {
	   	main_console->write("FPU support...");
		asm(R"ASM(
			mov %%cr0, %%eax
			and   $~2, %%eax /* clear EM (no emulation) */
			or     $1, %%eax /* set MP (#NM exception support) */
			or    $16, %%eax /* set NE (#MF exception handled internally) */
			mov %%eax, %%cr0
			fninit /* init FPU */
			)ASM" ::: "eax");
	   	main_console->write(" enabled\n");
}

NOINLINE void enableSSE() {
	   	main_console->write("SEE support...");
		asm(R"ASM(
			mov  %%cr4, %%eax
			/* or  $0x100, %%eax */ /* set OSFXSR (OS support FXSAVE/FXRSTOR) */
			or  $0x200, %%eax /* set OSXMMEXCPT (OS support #XF exception) */
			mov  %%eax, %%cr4
			
			pxor %%xmm0, %%xmm0 /* should pass */
			)ASM" ::: "eax");
	   	main_console->write(" enabled\n");
}

segmentation::Manager segmentationManager;
interrupt::Manager    interruptManager;

} // namespace kernel

NOINLINE int getZero() { return 0; }
NOINLINE int getDix() { return 10; }

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

	kernel::enableFPU();
	kernel::enableSSE();

	cxxruntime::Run running;
	
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

	std::cout("Kernel %running%...",
			std::color::green, std::color::ltgray) << std::endl;

	const int max = 3;
	for (int i = 1; i <= max; ++i)
	{
		std::cout("<%>\n", 10 / 0);
		std::cout << "Waiting wakeup..."
			<< std::format(" (%//%)", i, max) << std::endl;
		asm ("hlt");
	}

	std::cout("kernel stopping...\n");
}
