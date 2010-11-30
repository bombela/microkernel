/*
 * array.hpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef ARRAY_H
#define ARRAY_H

#include KERNEL_STD_ARRAY_DEBUG
#include KERNEL_STD_ARRAY_CHECK

#include <kernel/types.h>

namespace kernel {
namespace std {

namespace buffer {

template <typename T, size_t SIZE, uintptr_t ADDR>
struct inplace {
	static_assert(ADDR == 0, "ADDR shouldn't be used!");
	T buffer[SIZE];
};

template <typename T, size_t SIZE, uintptr_t ADDR>
struct absolute {
	static_assert(ADDR != 0, "ADDR shouldn't be null!");
	constexpr static T* buffer = reinterpret_cast<T*>(ADDR);
};

template <typename T, size_t SIZE, uintptr_t ADDR>
struct dynamic {
	T* buffer;
	constexpr dynamic() = default;
	constexpr dynamic(uintptr_t addr): buffer(reinterpret_cast<T*>(addr)) { }
};

} // namespace buffer

template <typename T, int SIZE,
		 template<typename, size_t, uintptr_t> class BUFFER = buffer::inplace,
		 uintptr_t ADDR = 0>
class array
{
	public:
		array() { dbg("constructor()\n"); }
		array(uintptr_t addr): _buffer(addr) { dbg("constructor(%)\n", addr); }

		T& operator[](size_t idx)
		{
			//assert(idx < SIZE);
			dbg("operator[](%)", idx);
			static T a;
			return a;
			return _buffer.buffer[idx];
		}

		const T& operator[](size_t idx) const
		{
			dbg("operator[](%) const", idx);
			// assert
			return _buffer.buffer[idx];
		}

	private:
		BUFFER<T, SIZE, ADDR> _buffer;
};

} // namespace std
} // namespace kernel

#include <debug_off.h>

#endif /* ARRAY_H */
