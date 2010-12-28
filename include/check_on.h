/*
 * check_on.h
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <basename.h>
#include <kernel/die.h>

#undef assert

#ifdef DEBUG_REENTRANT
#	include <kernel/console.h>
#	include <kernel/std/nbprinter.hpp>
#	define assert(__expr) do { if (not (__expr)) { \
		if (main_console) { \
			main_console->write(__FILE__ ":" \
					+ utility::basename(__FILE__)); \
			auto print = nbprinter::mkprinter<int>("0123456789"); \
			main_console->write(print(__LINE__)); \
			main_console->write(" "); \
			main_console->write(__PRETTY_FUNCTION__); \
			main_console->write(" - /!\\ ASSERTION ERROR /!\\ (" #__expr ")\n"); \
		} kernel::die(); } } while (0)
#else // !DEBUG_REENTRANT
#	include <kernel/debug.h>
#	define assert(__expr) do { if (not (__expr)) { \
			kernel::debug::printf(__FILE__ ":% % - /!\\ ASSERTION ERROR /!\\ (" \
			#__expr ")\n" + utility::basename(__FILE__), \
			__LINE__, __PRETTY_FUNCTION__); kernel::die(); } } while (0)
#endif // DEBUG_REENTRANT

#ifndef CHECK_ON
#	define CHECK_ON
#endif
