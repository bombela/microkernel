/*
 * check_on.h
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#undef assert
#define assert(__expr) do { if (not (__expr)) \
	kernel::debug::printf(__FILE__ ":% % - assertion error (" #__expr ")" \
		+ utility::basename(__FILE__), __LINE__, __PRETTY_FUNCTION__); \
	} while (0)
