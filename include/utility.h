/*
 * utility.h
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef UTILITY_H
#define UTILITY_H

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

#endif /* UTILITY_H */
