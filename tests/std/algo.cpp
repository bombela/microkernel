/*
 * fill.cpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

// Disable the gnu range access to prevent any conflit.
#define _GLIBCXX_RANGE_ACCESS_H

#include <test.h>
#include <kernel/std/array>
#include <kernel/std/algorithm>

using namespace ::std::kernel_std;

BOOST_AUTO_TEST_CASE(fill_iter)
{
	array<int, 42> a;

	fill(a.begin(), a.end(), 4);

	for (auto v : a)
		BOOST_CHECK(v == 4);
}

BOOST_AUTO_TEST_CASE(fill_range)
{
	array<int, 42> a;

	fill(a, 9);

	for (auto v : a)
		BOOST_CHECK(v == 9);
}

BOOST_AUTO_TEST_CASE(copy_iter)
{
	array<int, 42> a { 1, 5, 1 };
	array<int, 42> b;

	copy(a.begin(), a.end(), b.begin());

	for (unsigned i = 0; i < a.size(); ++i)
		BOOST_CHECK(a[i] == b[i]);
}

BOOST_AUTO_TEST_CASE(copy_range)
{
	array<int, 42> a { 1, 5, 1 };
	array<int, 42> b;

	copy(a, b);

	for (unsigned i = 0; i < a.size(); ++i)
		BOOST_CHECK(a[i] == b[i]);
}
