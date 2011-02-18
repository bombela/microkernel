/*
 * pagination.cpp
 * Copyright © 2011, François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <kernel/pagination.h>
#include <kernel/vga_console.h>

#include KERNEL_PAGINATION_DEBUG
#include KERNEL_PAGINATION_CHECK

namespace kernel {
namespace pagination {

void Manager::init(phymem::Manager* phymem)
{
	_phymem = phymem;

	dbg("pagination initializing identity mapping physical memory...");
	
	new (&_kernelContext) Context(_phymem);

	for (auto& p: _phymem->mem().cast<Page*>())
		_kernelContext.map(&p);

	_kernelContext.unmap(nullptr); // unmap for ptr null

	useKernelContext();
	
	dbg("activating CPU pagination...");

	asm volatile (R"ASM(
		movl %%cr0, %%eax
		orl $0x80010000, %%eax /* pagination + write protection enabled */
		movl %%eax,%%cr0

		/* flush prefetch */
		jmp 1f
		1:
		movl $2f, %%eax
		jmp *%%eax
		2: 
		)ASM" ::: "memory", "eax");

	dbg("page fault! ahah just kidding ;)");
	dbg("pagination initialized");
};

Manager::~Manager()
{
	dbg("disabling CPU pagination...");

	asm volatile (R"ASM(
		movl %%cr0, %%eax
		andl $~0x80010000, %%eax /* pagination + write protection disabled */
		movl %%eax,%%cr0

		/* flush prefetch */
		jmp 1f
		1:
		movl $2f, %%eax
		jmp *%%eax
		2: 
		)ASM" ::: "memory", "eax");
	dbg("pagination disabled");
}

void Manager::useContext(Context& context)
{
	dbg("switch to context %", context);

	_currentContext = &context;
	struct
	{
		uint32_t ignored1:        3;
		bool write_trough:        1;
		bool cache_disabled:      1;
		uint32_t ignored2:        7;
		uint32_t directory_addr: 20;

	} PACKED cr3{ 0, false, true, 0,
		reinterpret_cast<uint32_t>(
				context.getDirectoryAddr()
				) >> 12
	};

	asm volatile (R"ASM(
		movl %0, %%cr3
		)ASM" :: "r" (cr3)
	);

	dbg("switched to %", context);
}

Context Manager::newContext()
{
	Context context(_phymem);

	auto kmem = memory::range(&__b_kernel, &__e_kernel);
	auto vmem = VGAConsole::mem();
	
	for (auto& p: kmem.pages().cast<Page*>())
		context.map(&p);

	for (auto& p: vmem.pages().cast<Page*>())
		context.map(&p);
	
	context.map(&vaddr(_kernelContext.getDirectoryAddr()).page());

	return context;
}

} // namespace pagination
} // namespace kernel
