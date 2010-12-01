/*
 * debug.h
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <is_printable.hpp>
#include <enable_if.hpp>

namespace kernel {
namespace debug {

namespace details {

template <typename S, typename T>
void print_helper(S& os, const T&,
		typename enable_if<not is_printable<T, S>::value>::type = 0) {
	os << "<something>";
}

template <typename S, typename T>
void print_helper(S& os, const T& v,
		typename enable_if<is_printable<T, S>::value>::type = 0) {
	os << v;
}

} // namespace details

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
			//std::cout << arg1;
			details::print_helper(std::cout, arg1);
			printf(fmt, args...);
			return;
		}
		std::cout << *fmt++;
	}
}

} // namespace debug
} // namespace kernel

#endif /* DEBUG_H */
