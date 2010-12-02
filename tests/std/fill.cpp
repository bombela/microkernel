/*
 * fill.cpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

// Disable the gnu range access to prevent any conflit.
#define _GLIBCXX_RANGE_ACCESS_H

#include <test.h>

#include <kernel/std/array.hpp>
#include <kernel/std/algo.hpp>

using namespace kernel::std;

BOOST_AUTO_TEST_CASE(simple)
{
	array<int, 42> a;

	kernel::std::fill(a.begin(), a.end(), 4);

	for (auto v : a)
		BOOST_ASSERT(v == 4);
}

BOOST_AUTO_TEST_CASE(range)
{
	array<int, 42> a;

	kernel::std::fill(a, 9);

	for (auto v : a)
		BOOST_ASSERT(v == 9);
}
