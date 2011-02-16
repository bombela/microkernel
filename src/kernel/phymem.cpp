/*
 * phymem.cpp
 * Copyright © 2011, François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <kernel/phymem.h>
#include <kernel/memory.h>
#include <kernel/vga_console.h>

#include KERNEL_PHYMEM_DEBUG
#include KERNEL_PHYMEM_CHECK

namespace kernel {
namespace phymem {

void Manager::init(unsigned mem_lower_kb, unsigned mem_upper_kb)
{
	dbg("initializing");

	dbg("lower=%kb upper=%kb", mem_lower_kb, mem_upper_kb);
	auto video_mem_pages = VGAConsole::getInstance().memRange().pages();
	dbg("videomem pages=%", video_mem_pages);

	dbg("initialized");
}

} // namespace phymem
} // namespace kernel
