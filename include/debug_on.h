/*
 * debug_on.h
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <debug.h>
#include <basename.h>

#undef dbg

#define dbg(...) do { kernel::debug::printf(__FILE__ ":% % - " \
		+ utility::basename(__FILE__), __LINE__, __PRETTY_FUNCTION__); \
		kernel::debug::printf(__VA_ARGS__); } while (0)
