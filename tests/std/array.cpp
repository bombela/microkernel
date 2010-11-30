/*
 * array.cpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <test.h>

#include <kernel/std/array.hpp>
#include <iostream>

using namespace kernel::std;

BOOST_AUTO_TEST_CASE(construction)
{
	// inplace array
	// equivalent: int a[42];
	array<int, 42> a;

	// inplace array
	// equivalent: int b[42];
	array<int, 42, buffer::inplace> b;

	// absolute array
	// closely equivalent: int (const *c)[42] = 0x300;
	array<int, 42, buffer::absolute, 0x300> c;

	// dynamic array
	// closely equivalent: int (*d)[42];
	array<int, 42, buffer::dynamic> d;

	// dynamic array
	// closely equivalent: int (*e)[42] = 0x300;
	array<int, 42, buffer::dynamic> e(0x300);
}

BOOST_AUTO_TEST_CASE(inplace)
{
	array<int, 42> a;
	
	a[0] = 1;
	BOOST_ASSERT(a[0] == 1);
	
	a[41] = 2;
	BOOST_ASSERT(a[41] == 2);
	
	const array<int, 42> b;
	UNUSED int i = b[0];
}
