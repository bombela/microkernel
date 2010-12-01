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
	a[9] = 90;
	a[41] = 2;
	
	BOOST_ASSERT(a[0] == 1);
	BOOST_ASSERT(a[41] == 2);
	BOOST_ASSERT(a[9] == 90);
	
	const array<int, 42> b;
	UNUSED int i = b[0];

	BOOST_ASSERT(a.size() == 42);
}

BOOST_AUTO_TEST_CASE(absolute)
{
	array<int, 42, buffer::absolute, 1> a;
	
	BOOST_ASSERT(a.size() == 42);
}

BOOST_AUTO_TEST_CASE(dynamic)
{
	int data[42];
	array<int, 42, buffer::dynamic> a(&data);
	
	a[0] = 1;
	a[9] = 90;
	a[41] = 2;
	
	BOOST_ASSERT(a[0] == 1);
	BOOST_ASSERT(a[41] == 2);
	BOOST_ASSERT(a[9] == 90);
	
	const array<int, 42> b;
	UNUSED int i = b[0];

	BOOST_ASSERT(a.size() == 42);
}

BOOST_AUTO_TEST_CASE(dynamic2)
{
	int data1[42];
	data1[4] = 99;
	data1[12] = 102;
	
	int data2[42];
	data2[6] = 33;
	data2[14] = 9;

	array<int, 42, buffer::dynamic> a(&data1);
	
	BOOST_ASSERT(a[4] == 99);
	BOOST_ASSERT(a[12] == 102);
	
	//a.set_addr();
	a = array<int, 42, buffer::dynamic>(&data2);

	BOOST_ASSERT(a[6] == 33);
	BOOST_ASSERT(a[14] == 9);
}

BOOST_AUTO_TEST_CASE(iteration)
{
	array<int, 42> a;

	for (int i = 0; i < 42; ++i)
		a[i] = i * 2;
	for (int i = 0; i < 42; ++i)
		BOOST_ASSERT(a[i] == i * 2);
}
