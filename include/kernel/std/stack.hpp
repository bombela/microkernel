/*
 * stack.hpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef STACK_H
#define STACK_H

#include <kernel/std/array.hpp>

#include KERNEL_STD_STACK_DEBUG
#include KERNEL_STD_STACK_CHECK

namespace kernel {
namespace std {

template <typename T, size_t SIZE = 8>
class stack
{
	public:
		typedef T value_type;

		stack(): _size(0) { dbg("stack()"); }
		
		value_type&       top() {
			assert(size() > 0);
			dbg("top() -> %", _array[_size - 1]);
			return _array[_size - 1];
		}
		const value_type& top() const {
			assert(size() > 0);
			dbg("top() -> %", _array[_size - 1]);
			return _array[_size - 1];
		}

		void pop() {
			assert(size() > 0);
			--_size;
			dbg("pop() newsize %", _size);
		}
		value_type pop_get() {
			dbg("pop_get...");
			value_type r = top(); pop();
			dbg("...pop_get -> %", r);
			return r; }
	
		void push(const T& v) {
			dbg("push(%)", v);
			assert(size() < _array.size());
			_array[_size++] = v;
		}

		size_t size()  const { return _size; }
		bool   empty() const { return size() == 0; }
		
	private:
		array<T, SIZE, buffer::inplace> _array;
		size_t                          _size;
};

} // namespace std
} // namespace kernel

#include <check_off.h>
#include <debug_off.h>

#endif /* STACK_H */
