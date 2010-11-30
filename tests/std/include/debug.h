/*
 * debug.h
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>

namespace kernel {
namespace debug {

template <typename... Args>
void printf(const char* fmt)
{
	std::cout << fmt << std::flush;
}

template <typename T, typename... Args>
void printf(const char* fmt, const T& arg1, const Args&... args)
{
	while (*fmt)
	{
		if (*fmt == '%' && *++fmt != '%')
		{
			std::cout << arg1;
			printf(fmt, args...);
			return;
		}
		std::cout << *fmt++;
	}
}

} // namespace debug
} // namespace kernel

#endif /* DEBUG_H */
