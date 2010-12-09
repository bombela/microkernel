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
#include <kernel/std/ios>

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
	typedef P<S, T> printer_t;
	typedef formatter_arg<S, P, Args...> next_t;

	const T& arg;
	constexpr formatter_arg(const T& a, const Args&... args):
		formatter_arg<S, P, Args...>(args...), arg(a) {}
	
	constexpr S& print(S& os, const char* fmt) const
	{
		const char* fmt_start = fmt;
		while (*fmt)
		{
			if (*fmt == '%' && *++fmt != '%')
			{
				ios_flags flags = os.flags;
				do
				{
					switch (*fmt)
					{
						case 'd': os.flags.setbase(numbase::dec); break;
						case 'o': os.flags.setbase(numbase::oct); break;
						case 'b': os.flags.setbase(numbase::bin); break;
						case 'x': os.flags.setbase(numbase::hex); break;
						case 'X': os.flags.setbase(numbase::hex);
								  os.flags.setuppercase(true); break;
						case 'a': os.flags.setboolalpha(false); break;
						case 'A': os.flags.setboolalpha(true); break;
						case 'c': os.flags.setcharalpha(false); break;
						case 'C': os.flags.setcharalpha(true); break;
						case 'u': os.flags.setuppercase(false); break;
						case 'U': os.flags.setuppercase(true); break;
						
						case '/':
							++fmt;
						default:
							os.rdbuf()->write(fmt_start, fmt - fmt_start - 1);
							printer_t::print(os, arg);
							os.flags = flags;
							return next_t::print(os, fmt);
					}
				} while (++fmt);
			}
			++fmt;
		}
		os << fmt_start;
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
		return formatter_arg<S, P, Args...>::print(os, fmt);
	}

	friend S& operator<<(S& os, const formatter& fmter) {
		return fmter.print(os);
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
