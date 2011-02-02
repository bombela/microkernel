/*
 * check_on.h
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <basename.h>
#include <kernel/die.h>

#ifdef DEBUG_REENTRANT
#	include <kernel/std/nbprinter>
#	include <kernel/console.h>
#	undef assert
#	define assert(__expr) do { if (not (__expr)) { \
		if (::kernel::main_console) { \
			::kernel::main_console->write(__FILE__ ":" \
					+ utility::basename(__FILE__)); \
			auto print = nbprinter::mkprinter<int>("0123456789"); \
			::kernel::main_console->write(print(__LINE__)); \
			::kernel::main_console->write(" "); \
			::kernel::main_console->write(__PRETTY_FUNCTION__); \
			::kernel::main_console->write(" - /!\\ ASSERTION ERROR /!\\ (" #__expr ")\n"); \
		} ::kernel::die(); } } while (0)
#else // !DEBUG_REENTRANT
#	include <kernel/debug.h>
#	undef assert
#	define assert(__expr) do { if (not (__expr)) { \
			::kernel::debug::printf(__FILE__ ":% % - /!\\ ASSERTION ERROR /!\\ (" \
			#__expr ")\n" + utility::basename(__FILE__), \
			__LINE__, __PRETTY_FUNCTION__); ::kernel::die(); } } while (0)
#endif // DEBUG_REENTRANT

#ifndef CHECK_ON
#	define CHECK_ON
#endif
