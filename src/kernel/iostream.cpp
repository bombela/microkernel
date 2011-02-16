/*
 * iostream.cpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <kernel/std/iostream>
#include <kernel/std/streambuf>
#include <kernel/console.h>
#include <attributes.h>

#include KERNEL_STD_IOSTREAM_DEBUG
#include KERNEL_STD_IOSTREAM_CHECK

using namespace kernel;

namespace std {
namespace kernel_std {

class streambuf_console_impl: public streambuf
{
	public:
		~streambuf_console_impl() {
#ifdef DEBUG_ON
			if (main_console)
				main_console->write("streambuf_console_impl destruction\n");
#endif
		}
		
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
		virtual void write(const char* s, size_t len) {
			assert(main_console != 0);
			main_console->write(s, len);
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

const bool cout_initialized = false;
INIT_PRIORITY(65535) streambuf_console_impl streambuf_console;
INIT_PRIORITY(65534) ostream cout(&streambuf_console);

INIT_PRIORITY(65533) struct ostream_init {
	ostream_init() { const_cast<bool&>(cout_initialized) = true; }
	~ostream_init() { const_cast<bool&>(cout_initialized) = false; }
} ostream_initialization;

} // namespace kernel_std
} // namespace std
