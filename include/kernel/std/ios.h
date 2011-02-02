/* vim: ft=cpp
 * Local Variables:
 * mode:c++
 * End:
 * ios
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef IOS_H
#define IOS_H

#include <kernel/types.h>
#include <kernel/std/streambuf.h>

namespace kernel {
namespace std {

enum numbase {
	human     = 0U,
	dec       = 1U,
	oct       = 2U,
	hex       = 3U,
	bin       = 4U,
	numbase_max = 7U,
};

enum fmt {
	boolalpha = 1U << 4,
	charalpha = 1U << 5,
	uppercase = 1U << 6,
};

class ios_flags
{
	public:
		constexpr ios_flags(): _flags(_mkflags(human, charalpha, boolalpha)) {}

		inline void setbase(numbase nb) {
			_flags = (uint8_t(_flags) bitand ~numbase_max) bitor uint8_t(nb); }

		inline void setboolalpha(bool b = true) {
			setbit(fmt::boolalpha, b);
		}
		inline void setcharalpha(bool b = true) {
			setbit(fmt::charalpha, b);
		}
		inline void setuppercase(bool b = true) {
			setbit(fmt::uppercase, b);
		}

		template <typename... Args>
			inline void set(Args... args) {
				_flags = _mkflags(args...);
			}
		inline void reset() { set(human, charalpha, boolalpha); }

		inline constexpr numbase getbase() const {
			return numbase(uint8_t(_flags) bitand numbase_max);
		}

		inline constexpr bool isboolalpha() const {
			return uint8_t(_flags) & uint8_t(fmt::boolalpha);
		}

		inline constexpr char ischaralpha() const {
			return uint8_t(_flags) & uint8_t(fmt::charalpha);
		}

		inline constexpr bool isuppercase() const {
			return uint8_t(_flags) & uint8_t(fmt::uppercase);
		}

	private:
		uint8_t _flags;
		/*
		 *   8 7 6 5 4 3 2 1 0
		 * not  |u|c|b| num  |
		 * used |p|a|a| base |
		 */

		inline void setbit(uint8_t bit, bool b) {
			if (b)
				_flags = uint8_t(_flags) bitor uint8_t(bit);
			else
				_flags = uint8_t(_flags) bitand (~uint8_t(bit));
		}

		inline constexpr uint8_t _mkflags() const { return 0; }
		inline constexpr uint8_t _mkflags(numbase nb) const {
			return uint8_t(nb);
		}
		inline constexpr uint8_t _mkflags(fmt f) const { return uint8_t(f); }
		template <typename T, typename... Args>
			inline constexpr uint8_t _mkflags(T a1, Args... args) const {
				return _mkflags(a1) bitor _mkflags(args...);
			}
};

class ios
{
	public:
		ios_flags flags;
		
		inline constexpr ios(streambuf* sb): _streambuf(sb) {}
		inline constexpr streambuf* rdbuf() const { return _streambuf; }
		inline streambuf* rdbuf(streambuf* sb) {
			streambuf* old = _streambuf;
			_streambuf = sb;
			return old;
		}
		inline void setcolor(color c) { _streambuf->setcolor(c); }

		struct sentry
		{
			ios&      ios_ref;
			ios_flags flags;
			explicit constexpr sentry(ios& ios):
				ios_ref(ios), flags(ios.flags) { }
			~sentry() { ios_ref.flags = flags; }
		};
	
	protected:
		streambuf* _streambuf;
};

} // namespace std
} // namespace kernel

#endif /* IOS_H */
