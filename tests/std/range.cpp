/*
 * range.cpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

// Disable the gnu range access to prevent any conflit.
#define _GLIBCXX_RANGE_ACCESS_H

#include <test.h>

#include <kernel/std/range.hpp>
#include <kernel/std/array.hpp>

using namespace kernel::std;

BOOST_AUTO_TEST_CASE(basic)
{
	typedef array<int, 3> my_array;
	
	my_array a { 1, 2, 3 };

	my_array::iterator it = begin(a);
	BOOST_ASSERT(*it == 1);
	
}

BOOST_AUTO_TEST_CASE(readonly)
{
	typedef array<int, 3> my_array;
	
	my_array a;

	const my_array::const_iterator& it2 = end(static_cast<const my_array&>(a));
	BOOST_ASSERT(it2 == a.end());
}
