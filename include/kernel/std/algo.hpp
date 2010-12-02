/*
 * algo.hpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef ALGO_H
#define ALGO_H

#include <kernel/std/range.hpp>

namespace kernel {
namespace std {

template <typename IT, typename T>
	void fill(IT first, IT last, const T& value)
	{
		while (first != last)
			*first++ = value;
	}

template <typename C, typename T>
	void fill(C& container, const T& value)
	{
		fill(begin(container), end(container), value);
	}

} // namespace std
} // namespace kernel

#endif /* ALGO_H */
