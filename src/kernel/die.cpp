/*
 * die.cpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

// don't use the include, to remove the stupid gcc warning:
// warning: ‘noreturn’ function does return
//#include <kernel/die.h>

// we disable the debug and check use of the array,
// since we don't want any debug or assert.
#undef  KERNEL_STD_ARRAY_DEBUG
#define KERNEL_STD_ARRAY_DEBUG "debug_off.h"
#undef  KERNEL_STD_ARRAY_CHECK
#define KERNEL_STD_ARRAY_CHECK "check_off.h"
#include <kernel/std/array.hpp>

#include <attributes.h>
#include <kernel/console.h>
#include <kernel/vga_console.h>

namespace kernel {

struct vga_char {
	char c;
	char attr;
} PACKED;

void panic() {
	static const char panic_msg[] = " *** /!\\ Kernel Panic /!\\ ***";
	
	const size_t columns = 80;
	const size_t rows    = 25;
	const int    red     = 4;

	kernel::std::array<vga_char, columns * rows,
		kernel::std::buffer::absolute, 0xB8000> video_mem;

	auto it_vmem = video_mem.begin() + columns - sizeof(panic_msg);
	for(char c : panic_msg)
		*it_vmem++ = { c, red };

	asm volatile (R"(
		cli
	1:
		hlt
		jmp 1b
	)");
}

extern "C" void __kernel_print_stop_msg() {
	static const char stop_msg[] = " *** /!\\ Kernel Stopped /!\\ ***";
	
	const size_t columns = 80;
	const size_t rows    = 25;
	const int    orange  = 6;

	kernel::std::array<vga_char, columns * rows,
		kernel::std::buffer::absolute, 0xB8000> video_mem;

	auto it_vmem = video_mem.begin() + columns - sizeof(stop_msg);
	for(char c : stop_msg)
		*it_vmem++ = { c, orange };
}

void die()
{
	static const char panic_msg[] = "/Kernel Panic";
	if (main_console)
		main_console->write(panic_msg);
	else
		VGAConsole::getInstance().write(panic_msg);
	panic();
}

} // namespace kernel
