/*
 * array.hpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef ARRAY_H
#define ARRAY_H

#include KERNEL_STD_ARRAY_DEBUG

namespace kernel {
namespace std {

template <typename T, int SIZE>
class array
{
	public:
		array() { dbg("constructor"); }

	private:
		T _buffer[SIZE];
};

} // namespace std
} // namespace kernel

#include <debug_off.h>

#endif /* ARRAY_H */
