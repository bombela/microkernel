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

extern "C" memory::Page __b_kernel;
extern "C" memory::Page __e_kernel;

void Manager::init(unsigned mem_lower_kb, unsigned mem_upper_kb)
{
	dbg("initializing");

	auto mem_lower = memory::kilo(mem_lower_kb);
	auto mem_upper = memory::kilo(mem_upper_kb);

	dbg("lower=% upper=%", mem_lower, mem_upper);

	auto vmem = VGAConsole::getInstance().memRange();

	memory::PageRange lowmem(memory::octet(0),
			memory::kilo(mem_lower_kb));
	memory::PageRange biosmem(memory::kilo(mem_lower_kb),
			memory::mega(1));
	memory::PageRange highmem(memory::mega(1),
			memory::kilo(mem_upper_kb));
	auto kmem = memory::range(&__b_kernel, &__e_kernel).aligned();
	
	dbg("lowmem=% biosmem=% vmem=% highmem=% kmem(%)=% ",
			lowmem, biosmem, vmem, highmem,
			memory::page(kmem.size()), kmem);

	auto pages = lowmem.joined(biosmem).joined(vmem.pages())
		.joined(highmem).joined(kmem);

	dbg("total pages=% =%", pages, memory::page(pages.size()));
	_pages.resize(pages.size());

	_map.relocate(&__e_kernel);
	_map.resize(pages.size());
	auto pmgnt = _map.range();

	dbg("physmem managment cost=% on %",
			memory::octet(pmgnt.cast<uint8_t>().size()), pmgnt
	
	   );

	free(pages);
	use(pmgnt.pages());
	use(vmem.pages());
	use(biosmem);
	use(kmem);

	dbg("initialized");
}

void Manager::printMemUsage() const {
	size_t cntused = _map.cntset();
	std::cout("phymem usage %/%% (%//% pages)\n",
			cntused * 100 / _pages.size(),
			cntused, _pages.size()
			);
}

} // namespace phymem
} // namespace kernel
