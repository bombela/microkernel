/*
 * formatter.hpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef FORMATTER_H
#define FORMATTER_H

#include <is_printable.hpp>
#include <enable_if.hpp>

namespace kernel {
namespace std {
namespace formatter {

namespace arg_printer {

	template<typename S, typename T>
	struct simple {
		static inline constexpr void print(S& os, const T& arg) { os << arg; }
	};

	template <typename S, typename T>
	struct nofail {
		static inline constexpr void print(S& os, const T&,
				typename enable_if<not is_printable<T, S>::value>::type = 0) {
			os << "<something>";
		}

		static inline constexpr void print(S& os, const T& arg,
				typename enable_if<is_printable<T, S>::value>::type = 0) {
			os << arg;
		}
	};

} // namespace arg_printer

template <typename S, template <typename, typename> class P, typename... Args>
struct formatter_arg;

template <typename S, template <typename, typename> class P>
struct formatter_arg<S, P>
{
	S& print(S& os, const char* fmt) const {
		return os << fmt;
	}
};

template <typename S, template <typename, typename> class P,
		 typename T, typename... Args>
struct formatter_arg<S, P, T, Args...>: formatter_arg<S, P, Args...>
{
	const T& arg;
	constexpr formatter_arg(const T& a, const Args&... args):
		formatter_arg<S, P, Args...>(args...), arg(a) {}
	
	constexpr S& print(S& os, const char* fmt) const {
		while (*fmt)
		{
			if (*fmt == '%' && *++fmt != '%')
			{
				P<S, T>::print(os, arg);
				return formatter_arg<S, P, Args...>::print(os, fmt);
			}
			os << *fmt++;
		}
		return os;
	}
};

template <typename S, template <typename, typename> class P, typename... Args>
struct formatter: formatter_arg<S, P, Args...>
{
	const char* fmt;
	constexpr formatter(const char* fmt, const Args&... args):
		formatter_arg<S, P, Args...>(args...), fmt(fmt) {}

	constexpr S& print(S& os) const {
		formatter_arg<S, P, Args...>::print(os, fmt);
		return os;
	}
};

template <typename S,
		 template <typename, typename> class P, typename... Args>
inline constexpr formatter<S, P, Args...> mkformatter(const char* fmt,
		const Args&... args) {
	return formatter<S, P, Args...>(fmt, args...);
}

template <typename S,
		 template <typename, typename> class P = arg_printer::simple,
		 typename... Args>
inline constexpr S& print(S& os, const char* fmt, const Args&... args) {
	return mkformatter<S, P, Args...>(fmt, args...).print(os);
}

} // namespace formatter
} // namespace std
} // namespace kernel

#endif /* FORMATTER_H */
