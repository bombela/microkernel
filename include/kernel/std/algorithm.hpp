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

template <typename IT1, typename IT2>
	void copy(IT1 first, IT1 last, IT2 out)
	{
		while (first != last)
			*out++ = *first++;
	}

template <typename C1, typename C2>
	void copy(const C1& src, C2& out)
	{
		copy(begin(src), end(src), begin(out));
	}

} // namespace std
} // namespace kernel

#endif /* ALGO_H */
