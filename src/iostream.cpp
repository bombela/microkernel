/*
 * iostream.cpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <kernel/std/iostream>
#include <kernel/std/streambuf>
#include <kernel/console.h>

#include KERNEL_STD_IOSTREAM_DEBUG
#include KERNEL_STD_IOSTREAM_CHECK

namespace kernel {
namespace std {

class streambuf_console_impl: public streambuf
{
	public:
		~streambuf_console_impl() {
#ifdef DEBUG_ON
			if (main_console)
				main_console->write("streambuf_console_impl destruction\n");
#endif
		}
#ifndef DEBUG_ON
		constexpr
#endif
			streambuf_console_impl() {
#ifdef DEBUG_ON
			if (main_console)
				main_console->write("streambuf_console_impl initialization\n");
#endif
		}
		virtual void write(const char* s) {
			assert(main_console != 0);
			main_console->write(s);
		}
		virtual void write(char c) {
			assert(main_console != 0);
			main_console->write(c);
		}
		virtual void setcolor(color c) {
			assert(main_console != 0);
			main_console->setAttr({Console::Color(c)});
		}
};

INIT_PRIORITY(65535) streambuf_console_impl streambuf_console;
INIT_PRIORITY(65534) ostream cout(&streambuf_console);

} // namespace std
} // namespace kernel
