/*
 * debug.h
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef DEBUG_H
#define DEBUG_H

#include <kernel/std/iostream>
#include <kernel/std/formatter.hpp>

namespace kernel {
namespace debug {

template <typename... Args>
void printf(const char* fmt, const Args&... args)
{
	if (kernel::std::cout_initialized)
		kernel::std::formatter::print<kernel::std::ostream,
			kernel::std::formatter::arg_printer::nofail, Args...>(
					kernel::std::cout, fmt, args...);
}

} // namespace debug
} // namespace kernel

#endif /* DEBUG_H */
