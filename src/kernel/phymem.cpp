/*
 * phymem.cpp
 * Copyright © 2011, François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <kernel/phymem.h>
#include <kernel/vga_console.h>

#include KERNEL_PHYMEM_DEBUG
#include KERNEL_PHYMEM_CHECK

namespace kernel {
namespace phymem {

void Manager::init(unsigned mem_lower_kb, unsigned mem_upper_kb)
{
	dbg("initializing");

	auto mem_lower = memory::kilo(mem_lower_kb);
	auto mem_upper = memory::kilo(mem_upper_kb);

	dbg("lower=% upper=%", mem_lower, mem_upper);

	memory::PageRange lowmem(memory::octet(0),
			memory::kilo(mem_lower_kb));
	memory::PageRange biosmem(memory::kilo(mem_lower_kb),
			memory::mega(1));
	memory::PageRange highmem(memory::mega(1),
			memory::kilo(mem_upper_kb));
	auto vmem = VGAConsole::getInstance().memRange().pages();

	dbg("lowmem=% biosmem=% highmem=% vmem=%",
			lowmem, biosmem, highmem, vmem);

	auto a = memory::range(0, 50);
	auto w = a;
	auto b = memory::range(40, 10);
	auto c = a.split(b);
	auto j = b.joined(c).joined(a);

	dbg("w=%d a=%d b=%d c=%d j=%d", w, a, b, c, j);

	dbg("initialized");
}

} // namespace phymem
} // namespace kernel
