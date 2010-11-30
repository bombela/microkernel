/*
 * array.cpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <kernel/array.hpp>

#include <test.h>
#include <iostream>

using namespace kernel::std;

BOOST_AUTO_TEST_CASE(construction)
{
	array<int, 42> top;
}

BOOST_AUTO_TEST_CASE(destruction)
{
}
