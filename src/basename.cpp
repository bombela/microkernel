/*
 * basename.cpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <basename.h>

namespace utility {

int basename(const char* filepath)
{
	int i = 0;
	while (filepath[i])
		++i;
	while (i >= 0 && filepath[i] != '/')
		--i;
	return i + 1;
}

} // namespace utility
