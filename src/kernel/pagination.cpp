/*
 * pagination.cpp
 * Copyright © 2011, François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <kernel/pagination.h>

#include KERNEL_PAGINATION_DEBUG
#include KERNEL_PAGINATION_CHECK

namespace kernel {
namespace pagination {

void Manager::init() {
	dbg("Initializing pagination...");

	_directory.relocate(palloc());
	std::fill(_directory, DirectoryEntry());

	return;

	_phymem->usedPageApply([](memory::Page& p) {
		// identity map
	});

	asm volatile (R"ASM(
		movl %0, %%cr3
		movl %%cr0, %%eax
		orl $0x80010000, %%eax /* pagination + write protection enabled */
		movl %%eax,%%cr0
		
		/* flush prefetch */
		jmp 1f
		1:
		movl $2f, %%eax
		jmp *%%eax
		2: 
		)ASM" :: "r" (2) : "eax");

	dbg("Pagination initialized");
};

Page* Manager::identity_map(memory::Page* pp)
{
	auto ppa = vaddr(pp);
	assert(ppa.is_aligned());

	auto& de = _directory[ppa.directory()];
	TableEntry* te;

	if (de.present)
	{
		
	}
	else
	{
		/*te = palloc();
		de.setTable(te);
		de.present = true;
		te[0].setAddr(te);
		te[0].present = 1;*/
	}
}

} // namespace pagination
} // namespace kernel
