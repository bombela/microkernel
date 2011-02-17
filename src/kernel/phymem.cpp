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

	auto lowmem = memory::PageRange(memory::octet(0),
			memory::kilo(mem_lower_kb)).aligned();
	auto biosmem = memory::PageRange(memory::kilo(mem_lower_kb),
			memory::mega(1)).aligned();
	auto highmem = memory::PageRange(memory::mega(1),
			memory::kilo(mem_upper_kb)).aligned();
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

	_map.clear();
	use(_pages[0].page); // lock first page
	use(pmgnt.pages());
	use(biosmem.joined(vmem.pages()));
	use(kmem);

	rebuildFreeList();

	dbg("initialized");
}

void Manager::printMemUsage() const {
	auto print = [this] (size_t cntused) {
	std::cout("phymem usage %/%% (%//% pages) (%//%)\n",
			cntused * 100 / _pages.size(),
			cntused, _pages.size(),
			memory::page(cntused), memory::page(_pages.size())
			);
	};
	size_t cntused1 = _map.cntset();
	print(cntused1);
	size_t cntused2 = _pages.size() - cntFreePage();
	assert(cntused1 == cntused2);
}

void Manager::rebuildFreeList()
{
	size_t cnt = 0;
	Page* prev = 0;
	bool free_begin_set = false;
	for (auto& p : _pages) {
		if (not used(p))
		{
			++cnt;
			if (not free_begin_set) {
				_free_begin = &p;
				free_begin_set = true;
			}
			else
				prev->next = &p;
			p.prev = prev;
			prev = &p;
		}
	}
	prev->next = 0;
}

size_t Manager::cntFreePage() const
{
	size_t cnt = 0;
	Page* p = _free_begin;
   	for (; p; p = p->next)
		++cnt;
	return cnt;
}

} // namespace phymem
} // namespace kernel
