/*
 * range.cpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <test.h>

#include <kernel/std/range>
#include <kernel/std/array>

using namespace ::std::kernel_std;

BOOST_AUTO_TEST_CASE(basic)
{
	typedef array<int, 3> my_array;
	
	my_array a { 1, 2, 3 };

	my_array::iterator it = begin(a);
	BOOST_CHECK(*it == 1);
	
}

BOOST_AUTO_TEST_CASE(readonly)
{
	typedef array<int, 3> my_array;
	
	my_array a;

	const my_array::const_iterator& it2 = end(static_cast<const my_array&>(a));
	BOOST_CHECK(it2 == a.end());
}
