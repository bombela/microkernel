/*
 * check_on.h
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <die.h>

#undef assert
#define assert(__expr) do { if (not (__expr)) { \
	kernel::debug::printf(__FILE__ ":% % - /!\\ ASSERTION ERROR /!\\ (" \
			#__expr ")\n" + utility::basename(__FILE__), \
			__LINE__, __PRETTY_FUNCTION__); kernel::die(); } } while (0)
