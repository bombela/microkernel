/*
 * formatter.hpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef FORMATTER_H
#define FORMATTER_H

namespace kernel {
namespace std {
namespace formatter {

template <typename S, typename... Args>
struct formatter_arg;

template <typename S>
struct formatter_arg<S>
{
	S& print(S& os, const char* fmt) const {
		return os << fmt;
	}
};

template <typename S, typename T, typename... Args>
struct formatter_arg<S, T, Args...>: formatter_arg<S, Args...>
{
	const T& arg;
	constexpr formatter_arg(const T& a, const Args&... args):
		formatter_arg<S, Args...>(args...), arg(a) {}
	
	constexpr S& print(S& os, const char* fmt) const {
		while (*fmt)
		{
			if (*fmt == '%' && *++fmt != '%')
			{
				os << arg;
				return formatter_arg<S, Args...>::print(os, fmt);
			}
			os << *fmt++;
		}
		return os;
	}
};

template <typename S, typename... Args>
struct formatter: formatter_arg<S, Args...>
{
	const char* fmt;
	constexpr formatter(const char* fmt, const Args&... args):
		formatter_arg<S, Args...>(args...), fmt(fmt) {}

	constexpr S& print(S& os) const {
		formatter_arg<S, Args...>::print(os, fmt);
		return os;
	}
};

template <typename S, typename... Args>
inline constexpr formatter<S, Args...> mkformatter(const char* fmt,
		const Args&... args) {
	return formatter<S, Args...>(fmt, args...);
}

template <typename S, typename... Args>
inline constexpr S& print(S& os, const char* fmt, const Args&... args) {
	return mkformatter<S, Args...>(fmt, args...).print(os);
}

} // namespace formatter
} // namespace std
} // namespace kernel

#endif /* FORMATTER_H */
