/*
 * debug.h
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <kernel/std/formatter>

namespace kernel {
namespace debug {

template <typename... Args>
void printf(const char* fmt, const Args&... args)
{
	if (::std::cout_initialized)
		::std::formatter::print< ::std::ostream,
			::std::formatter::arg_printer::nofail, Args...>(
					::std::cout, fmt, args...);
}

} // namespace debug
} // namespace kernel

#endif /* DEBUG_H */
