/*
 * stack.cpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <test.h>

#include <kernel/std/stack.hpp>

#include <kernel/ioports.h>

using namespace kernel::std;

BOOST_AUTO_TEST_CASE(construction)
{
	stack<int> a;
	stack<int, 16> b;

	BOOST_ASSERT(a.size() == 0);
	BOOST_ASSERT(b.size() == 0);
}

BOOST_AUTO_TEST_CASE(push_top)
{
	stack<int> a;

	a.push(2);
	BOOST_ASSERT(a.size() == 1);
	BOOST_ASSERT(a.top() == 2);
}

BOOST_AUTO_TEST_CASE(push_pop)
{
	stack<int, 3> a;

	a.push(1);
	a.push(2);
	a.push(3);
	BOOST_ASSERT(a.size() == 3);
	BOOST_ASSERT(a.top() == 3);
	a.pop();
	BOOST_ASSERT(a.top() == 2);
	a.pop();
	BOOST_ASSERT(a.top() == 1);
	a.pop();
	BOOST_ASSERT(a.size() == 0);
}

BOOST_AUTO_TEST_CASE(pop_out)
{
	stack<int> a;

	a.push(65);
	BOOST_ASSERT(a.top() == 65);
	BOOST_ASSERT(a.pop_get() == 65);
	BOOST_ASSERT(a.size() == 0);
}
