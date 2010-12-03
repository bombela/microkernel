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

		constexpr stack(): _idx(0) {}
		
		value_type&       top() { }
		const value_type& top() const { }

		void pop() { }
		size_t size()  const { return _idx; }
		bool   empty() const { return size() == 0; }
		
	private:
		array<T, SIZE, buffer::inplace> _container;
		size_t                          _idx;
};

} // namespace std
} // namespace kernel

#include <check_off.h>
#include <debug_off.h>

#endif /* STACK_H */
