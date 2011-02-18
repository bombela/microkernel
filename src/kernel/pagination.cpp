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
	dbg("pagination initializing...");

	_directory.relocate(palloc());
	std::fill(_directory, DirectoryEntry());
	dbg("directory=%", &_directory[0]);

	dbg("identity mapping physical memory...");
	for (auto& p: _phymem->mem().cast<Page*>())
		identity_map(&p);
	
	dbg("enabling CPU pagination...");

	struct
	{
		uint32_t ignored1:        3;
		bool write_trough:        1;
		bool cache_disabled:      1;
		uint32_t ignored2:        7;
		uint32_t directory_addr: 20;

	} PACKED cr3{ 0, false, true, 0,
		reinterpret_cast<uint32_t>(&_directory[0]) >> 12
	};

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
		)ASM" :: "r" (cr3) : "memory", "eax");

	dbg("page fault ! ahah just kidding ;)");
	dbg("pagination initialized");
};

void Manager::identity_map(const pagination::Page* pp)
{
	auto& page = resolveTableEntry(pp);
	assert(not page.present);
	page.setPage(pp);
	page.present = true;
}

TableEntry& Manager::resolveTableEntry(const pagination::Page* p)
{
	auto va = vaddr(p);
	assert(va.is_aligned());

	auto& table = _directory[va.directory()];
	if (not table.present)
	{
		// allocate a table page
		Table* tablePage = reinterpret_cast<Table*>(palloc());
		table.setTable(tablePage);
		table.present = true;
		std::fill(*tablePage, TableEntry());
	}
	return table[va.table()];
}

} // namespace pagination
} // namespace kernel
