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

}
