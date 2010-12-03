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
	
	BOOST_CHECK(a[0] == 1);
	BOOST_CHECK(a[41] == 2);
	BOOST_CHECK(a[9] == 90);
	
	const array<int, 42> b;
	UNUSED int i = b[0];

	BOOST_CHECK(a.size() == 42);
}

BOOST_AUTO_TEST_CASE(absolute)
{
	array<int, 42, buffer::absolute, 1> a;
	
	BOOST_CHECK(a.size() == 42);
}

BOOST_AUTO_TEST_CASE(dynamic)
{
	int data[42];
	array<int, 42, buffer::dynamic> a(&data);
	
	a[0] = 1;
	a[9] = 90;
	a[41] = 2;
	
	BOOST_CHECK(a[0] == 1);
	BOOST_CHECK(a[41] == 2);
	BOOST_CHECK(a[9] == 90);
	
	const array<int, 42> b;
	UNUSED int i = b[0];

	BOOST_CHECK(a.size() == 42);
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
	
	BOOST_CHECK(a[4] == 99);
	BOOST_CHECK(a[12] == 102);
	
	//a.set_addr();
	a = array<int, 42, buffer::dynamic>(&data2);

	BOOST_CHECK(a[6] == 33);
	BOOST_CHECK(a[14] == 9);
}

BOOST_AUTO_TEST_CASE(iteration)
{
	array<int, 42> a;

	for (unsigned i = 0; i < a.size(); ++i)
		a[i] = i * 2;
	for (unsigned i = 0; i < a.size(); ++i)
		BOOST_CHECK(a[i] == static_cast<int>(i * 2));
}

BOOST_AUTO_TEST_CASE(iterator_simple)
{
	array<int, 3> a { 1, 2, 3 };

	auto it = a.begin();
	BOOST_CHECK(*it == 1);
	++it;
	BOOST_CHECK(*it == 2);
	++it;
	BOOST_CHECK(*it == 3);
	++it;
	BOOST_CHECK(it == a.end());

	int i = 1;
	for (auto it = a.begin(); it != a.end(); ++it)
		BOOST_CHECK(*it == i++);
}

BOOST_AUTO_TEST_CASE(iterator)
{
	array<int, 42> a;

	for (unsigned i = 0; i < a.size(); ++i)
		a[i] = i * 3;

	int i = 0;
	for (auto it = a.begin(); it != a.end(); ++it)
		BOOST_CHECK(*it == i++ * 3);

	i = 0;
	for (auto c : a)
	{
		BOOST_CHECK(c == i * 3);
		++i;
	}
	BOOST_CHECK(i == 42);

	i = 0;
	for (const auto c : a)
	{
		BOOST_CHECK(c == i * 3);
		++i;
	}
	BOOST_CHECK(i == 42);
}

BOOST_AUTO_TEST_CASE(inplace_easy_init)
{
	array<int, 5> a { 10, 42, 100, 34, 13, 0 };

	int valid[] = { 10, 42, 100, 34, 13, 3 };

	for (unsigned i = 0; i < a.size(); ++i)
		BOOST_CHECK(a[i] == valid[i]);
}

BOOST_AUTO_TEST_CASE(iterator_complex)
{
	array<float, 4> a { 90, 100, 23, 1 };

	auto b = a.begin();
	BOOST_CHECK(*b == 90);
	BOOST_CHECK(*b++ == 90);
	BOOST_CHECK(*b == 100);
	BOOST_CHECK(*++b == 23);
	b += 1;
	BOOST_CHECK(*b == 1);
	b = b + 1;
	BOOST_CHECK(b == a.end());

	auto c = a.end();
	BOOST_CHECK(*--c == 1);
	BOOST_CHECK(*c-- == 1);
	BOOST_CHECK(*c == 23);
	c -= 1;
	BOOST_CHECK(*c == 100);
	c = c - 1;
	BOOST_CHECK(*c == 90);

	BOOST_CHECK(*(b - 2) == 23);
	BOOST_CHECK(*(c + 2) == 23);
	BOOST_CHECK((b - 2) == (c + 2));
}

BOOST_AUTO_TEST_CASE(inplace_cpy)
{
	array<int, 3> a { 2, 10, 50 };
	array<int, 3> b(a);

	BOOST_CHECK(b[0] == 2);
	BOOST_CHECK(b[1] == 10);
	BOOST_CHECK(b[2] == 50);
}

BOOST_AUTO_TEST_CASE(inplace_affect)
{
	array<int, 3> a { 2, 10, 50 };
	array<int, 3> b = { 1, 1, 3 };

	b = a;
	BOOST_CHECK(b[0] == 2);
	BOOST_CHECK(b[1] == 10);
	BOOST_CHECK(b[2] == 50);
}

BOOST_AUTO_TEST_CASE(inplace_it_init)
{
	array<int, 6> a { 10, 42, 100, 34, 13, 0 };
	array<int, 5> b(a.begin(), a.end());

	for (unsigned i = 0; i < b.size(); ++i)
		BOOST_CHECK(a[i] == b[i]);
}

BOOST_AUTO_TEST_CASE(dynamic_affect)
{
	int buf[4] = { 1, 2, 3, 10 };
	array<int, 4, buffer::dynamic> a(&buf);

	int buf2[4] = { 31, 42, 13, 610 };
	array<int, 4, buffer::dynamic> b(&buf2);

	b = a;
	for (unsigned i = 0; i < a.size(); ++i)
		BOOST_CHECK(a[i] == b[i]);
}

BOOST_AUTO_TEST_CASE(iterator_defer)
{
	struct Titi { 
		int getVal() const { return val; };
		Titi(int v): val(v) {}
		int val;
		Titi():val(-1){}
	};
	array<Titi, 4> a { 90, 100, 23, 1 };

	BOOST_CHECK(a.begin()->getVal() == 90);
	auto it = a.begin() + 2;
	BOOST_CHECK(it->getVal() == 23);
	BOOST_CHECK((it = it + 1)->getVal() == 1);
}

BOOST_AUTO_TEST_CASE(iterator_equality)
{
	typedef array<int, 3> my_array;

	my_array a = { 1, 2, 3 };

	my_array::const_iterator it1 = static_cast<const my_array&>(a).begin();
	my_array::iterator       it2 = a.begin();

	BOOST_CHECK(it1 == it2);
	BOOST_CHECK(it2 == it1);
}

BOOST_AUTO_TEST_CASE(iterator_const)
{
	typedef array<int, 3> my_array;

	my_array a = { 1, 2, 3 };
	my_array::const_iterator it1 = a.begin();
	my_array::iterator       it2 = a.begin();
	my_array::const_iterator itt1 = a.begin();
	my_array::iterator       itt2 = a.begin();

	BOOST_CHECK(it1 == it2);
	BOOST_CHECK(it2 == it1);

	it1 = it2;
	BOOST_CHECK(it1 == it2);
	BOOST_CHECK(it2 == it1);

	++itt2;
	BOOST_CHECK(itt1 <  itt2);
	BOOST_CHECK(itt2 > itt1);
	BOOST_CHECK(itt1 <= itt2);
	BOOST_CHECK(itt2 >= itt1);

	--itt2;
	BOOST_CHECK(itt1 >= itt2);
	BOOST_CHECK(itt2 <= itt1);

}

BOOST_AUTO_TEST_CASE(init_fill)
{
	array<int, 42> a(fill(99));

	for (auto i : a)
		BOOST_CHECK(i == 99);
}

BOOST_AUTO_TEST_CASE(init_fill_dynamic)
{
	int data[42];
	array<int, 42, kernel::std::buffer::dynamic> a(&data, fill(98));

	for (auto i : a)
		BOOST_CHECK(i == 98);
}
