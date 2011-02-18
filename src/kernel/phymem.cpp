/*
 * phymem.cpp
 * Copyright © 2011, François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <kernel/phymem.h>
#include <kernel/vga_console.h>
#include <iostream>

#include KERNEL_PHYMEM_DEBUG
#include KERNEL_PHYMEM_CHECK

namespace kernel {
namespace phymem {

extern "C" memory::Page __b_kernel;
extern "C" memory::Page __e_kernel;

void Manager::init(unsigned mem_lower_kb, unsigned mem_upper_kb)
{
	dbg("initializing");

	auto vmem = VGAConsole::getInstance().memRange();

	auto lowmem = memory::PageRange(memory::octet(0),
			memory::kilo(mem_lower_kb)).aligned();
	auto biosmem = memory::PageRange(memory::kilo(mem_lower_kb),
			memory::mega(1)).aligned();
	auto highmem = memory::PageRange(memory::mega(1),
			memory::kilo(mem_upper_kb)).aligned();
	auto kmem = memory::range(&__b_kernel, &__e_kernel).aligned();
	
	std::cout("lowmem=% biosmem=% vmem=% highmem=% kmem(%)=%\n",
			lowmem, biosmem, vmem, highmem,
			memory::page(kmem.size()), kmem);

	auto pages = lowmem.joined(biosmem).joined(vmem.pages())
		.joined(highmem).joined(kmem);

	dbg("total pages=% =%", pages, memory::page(pages.size()));
	_pages.resize(pages.size());

	_map.relocate(&__e_kernel);
	_map.resize(pages.size());
	auto pmgnt = _map.range();

	std::cout("physmem managment cost=% on %\n",
			memory::octet(pmgnt.cast<uint8_t>().size()), pmgnt
	   );

	_map.clear();
	_map.set(_pages[0].page.number()); // lock first page

	auto inituse = [this](const memory::PageRange& range) {
		assert(range.is_aligned());
		for (auto& p : range) {
			_map.set(p.number());
		}
	};
	inituse(pmgnt.pages());
	inituse(biosmem.joined(vmem.pages()));
	inituse(kmem);

	rebuildFreeList();

	_kmem = kmem;
	_vmem = vmem.pages();
	_biosmem = biosmem;

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
#ifdef CHECK_ON
	size_t cntused2 = _pages.size() - cntFreePage();
	if (cntused1 != cntused2)
	{
		std::cout("err(diff=%)->", cntused2 - cntused1);
		print(cntused2);
	}
	assert(cntused1 == cntused2);
#endif
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

void Manager::testAllocator() {
	// backup
	std::cout("test physical memory allocator...\n");
	printMemUsage();
	BoolArray<0x1,
		std::buffer::dynamic_resizable> map_backup;
	map_backup.relocate(  _map.range().pages().end() );
	map_backup.resize(_pages.size());

	// test
	{
		map_backup = _map;
		for (auto& p : map_backup.range().pages().cast<Page*>())
			_map.set(p.page.number());
		rebuildFreeList();
		printMemUsage();

		std::array<Page*, 256> pages;
		for (auto& i : pages)
			i = _alloc();
		
		for (auto& i : pages)
		{
			i->prev = (Page*)-1;
			i->next = (Page*)-1;
		}

		for (auto i : pages)
			_free(i);

		size_t trigger = 0;
		Page* p;
		while ((p = _alloc()) != 0)
		{
			if (++trigger > _pages.size() / 10)
			{
				printMemUsage();
				trigger = 0;
			}
			p->next = memory::Addr<Page*>(42);
			p->prev = memory::Addr<Page*>(42);
			p->page.data[42] = 42;
			p->page.data[0] = 42;
			p->page.data[memory::page_size - 1] = 42;
		}
		
		// restore
		printMemUsage();
		_map = map_backup;
		rebuildFreeList();
	}

	std::cout("test physical memory allocator succed!\n");
}

} // namespace phymem
} // namespace kernel
