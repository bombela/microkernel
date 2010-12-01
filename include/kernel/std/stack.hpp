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

namespace allocator {

template <typename T, size_t SIZE,
		 template<typename, size_t, uintptr_t> class BUFFER = buffer::inplace,
		 uintptr_t ADDR = 0>
class array
{
	public:

	private:
		::kernel::std::array<T, SIZE, BUFFER, ADDR> _array;
};

} // namespace allocator

template <typename T, typename Allocator = allocator::array<T, 8> >
class vector
{
	public:
		vector(const Allocator& allocator = Allocator()):
			_allocator(allocator) {}

	private:
		Allocator _allocator;
};

template <typename T, typename Container = std::vector<T>>
class stack
{
	public:
		typedef T value_type;

		constexpr stack(const Container& ctnr = Container()):
			_container(ctnr), _idx(0) {}
		
		value_type&       top() { }
		const value_type& top() const { }

		void pop() { }
		size_t size()  const { return _idx; }
		bool   empty() const { return size() == 0; }
		
	private:
		Container _container;
		size_t    _idx;
};

} // namespace std
} // namespace kernel

#include <check_off.h>
#include <debug_off.h>

#endif /* STACK_H */
