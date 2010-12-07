/*
 * mainconsole.cpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <kernel/console.h>
#include <kernel/vga_console.h>

namespace kernel {

Console* main_console = 0;

void main_console_init()
{
	main_console = &VGAConsole::getInstance();
}

} // namespace kernel
