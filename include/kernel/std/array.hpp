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
#ifdef KERNEL_STD_ARRAY_CHECK
	constexpr dynamic(): buffer(nullptr) { }
#else
	constexpr dynamic() = default;
#endif
	constexpr dynamic(uintptr_t addr): buffer(reinterpret_cast<T*>(addr)) { }
};

} // namespace buffer

template <typename T, int SIZE,
		 template<typename, size_t, uintptr_t> class BUFFER = buffer::inplace,
		 uintptr_t ADDR = 0>
class array
{
	public:
		array() { dbg("constructor()"); }
		array(uintptr_t addr): _buffer(addr) {
			dbg("constructor(uintptr_t %)", addr);
		}
		array(void* addr): _buffer(reinterpret_cast<uintptr_t>(addr)) {
			dbg("constructor(void* %)", addr);
		}

		T& operator[](size_t idx)
		{
			assert(idx < SIZE);
			assert(_buffer.buffer != 0);
			dbg("operator[](%) -> %", idx, _buffer.buffer[idx]);
			return _buffer.buffer[idx];
		}

		const T& operator[](size_t idx) const
		{
			assert(idx < SIZE);
			assert(_buffer.buffer != 0);
			dbg("operator[](%) const -> %", idx, _buffer.buffer[idx]);
			return _buffer.buffer[idx];
		}

		constexpr size_t size() const { return SIZE; }

		typedef       T* iterator;
		typedef const T* const_iterator;

		iterator begin() { return &_buffer.buffer[0]; }
		iterator end() { return &_buffer.buffer[SIZE - 1]; }
		
		const_iterator begin() const { return &_buffer.buffer[0]; }
		const_iterator end() const { return &_buffer.buffer[SIZE - 1]; }

	private:
		BUFFER<T, SIZE, ADDR> _buffer;
};

} // namespace std
} // namespace kernel

#include <debug_off.h>

#endif /* ARRAY_H */
