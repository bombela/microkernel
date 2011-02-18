/*
 * main.cpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <kernel/multiboot.h>
#include <kernel/console.h>
#include <kernel/die.h>
#include <kernel/segmentation.h>
#include <kernel/interrupt.h>
#include <kernel/pic.h>
#include <kernel/phymem.h>
#include <kernel/pagination.h>

#include <cxxruntime.h>
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

extern "C" uint8_t __b_kernel_boot_stack;
extern "C" uint8_t __e_kernel_boot_stack;
void printBootStackUsage()
{
	uint8_t* esp;
	asm("mov %%esp, %0":"=g"(esp));
	std::cout("kernel boot stack usage: %/%% (%//% Bytes)\n",
			(&__e_kernel_boot_stack - esp) * 100 /
			(&__e_kernel_boot_stack - &__b_kernel_boot_stack),
			&__e_kernel_boot_stack - esp,
			&__e_kernel_boot_stack - &__b_kernel_boot_stack
			);
}

segmentation::Manager   segmentationManager;
pic::Manager            picManager;
interrupt::Manager      interruptManager;
phymem::Manager         phymemManager;
pagination::Manager     paginationManager;

#ifdef STACK_USAGE_PATCH
extern "C" uint8_t* ret_addr[];
struct AutoPrintBootStackUsage
{
	static void trap(int, int, void** _eip)
	{
		uint8_t** eip = (uint8_t**)_eip;
		*(*eip -1) = 0xC3;
		--*eip;

		if (std::cout_initialized)
			printBootStackUsage();
	}

	NOINLINE static void init()
	{
		std::cout("Auto print stack usage, patching kernel...");
		kernel::interruptManager.setHandler(3, &trap);

		unsigned cnt = 0;
		for (uint8_t** i = ret_addr; *i; ++i)
		{
			uint8_t*& addr = *i;
			uint8_t& instr = *addr;
			if (instr != 0xC3)
			{
				std::cout("Instruction differ, you should"
						" update the database with 'make genretidx' !\n");
				continue;
			}
			uint8_t* start = (uint8_t*)&init;
			uint8_t* end = start + 10000000;
			if ((addr >= start) and (addr < end))
				continue;
			instr = 0xCC;
			++cnt;
		}
		std::cout(" done, % ret patched.\n", cnt);
	}

	AutoPrintBootStackUsage()
	{
		init();
	}
} _apbs ;
#endif

extern "C" {
	uint32_t esp;
}
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

	enableFPU();
	enableSSE();

	cxxruntime::Run running;
	
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		std::cout("%Not loaded by a multiboot compliant loader%\n",
				std::color::red, std::color::ltgray);
		die();
	}

	std::cout("Mem = %MB (upper limit = %xkB)\n",
			(mbi->mem_upper >> 10), mbi->mem_upper);

	phymemManager.init(mbi->mem_lower, mbi->mem_upper);
	paginationManager.init(&phymemManager);

	struct APICVersionRegister {
		uint8_t version;
		uint8_t reserved1;
		uint8_t maxlvtentry;
		uint8_t supportsupressEOIbroadcast:1;
		uint8_t reserved2:7;
	} PACKED const * avr
	= reinterpret_cast<APICVersionRegister*>(0xFEE00030);

	auto& c = paginationManager.kernelContext();
	auto am = c.map(
			&pagination::vaddr(avr).page(),
			&phymem::paddr(avr).page()
			);
	std::cout("APIC version=%c maxlvtentry=%c\n",
			avr->version, avr->maxlvtentry);
	c.unmap(am);

	auto old =
		interruptManager.getHandler(picManager.irq2int(0));

	int cnt = 0;
	interruptManager.setHandler(picManager.irq2int(0),
		[&old, &cnt](int i, int, void**) {
			printBootStackUsage();

			if (++cnt > 10)
			{
				std::cout("stop playing!");
				interruptManager.setHandler(
					picManager.irq2int(0),
					[&old, &cnt](int i, int, void**) {
					std::cout(".");
					picManager.eoi(picManager.int2irq(i));
					});
			}

			picManager.eoi(picManager.int2irq(i));
			});
	//picManager.enable(0);

	printBootStackUsage();
	phymemManager.printMemUsage();
	
	{
		auto old =
			interruptManager.getHandler(14);

		interruptManager.setHandler(14,
				[](int, int err, void** eip) {
					std::cout(" nullptr deref catched! err=%\n", err);
					asm ("movl $after, %0" : "=m"(*eip));
				});

		std::cout("try to catch nullptr access...");
		asm (R"(
			movl $42, 0 /* write to address 0 */
			after:
			)");
		interruptManager.setHandler(14, old);
	}

	{
		auto old =
			interruptManager.getHandler(14);

		interruptManager.setHandler(14,
				[](int, int err, void** eip) {
					std::cout("yes, page fault!\n");
					asm ("movl $after2, %0" : "=m"(*eip));
				});

		auto c = paginationManager.newContext();

		paginationManager.useContext(c);

		std::cout("try the physical memory allocator while using"
			   "a fresh context: ");

		void (*test)() = [] {
			phymem::Page* p = phymemManager.alloc();
			phymemManager.free(p);
		};

		asm (R"(
			pusha
			movl %%esp, (esp)
			call *%0
			after2:
			mov (esp), %%esp
			popa
			)" :: "r" (test));
		paginationManager.useKernelContext();
		interruptManager.setHandler(14, old);

		std::cout("try again, but using kernel context... ");
		test();
		std::cout("success\n");
	}
	
	std::cout("Kernel %running%...",
			std::color::green, std::color::ltgray) << std::endl;
	
#if 0
	interruptManager.testInterrupts();
	phymemManager.testAllocator();
#endif

	//for (;;) asm ("hlt");
	std::cout("kernel stopping...\n");
}

//extern "C" uint32_t esp;

} // namespace kernel
