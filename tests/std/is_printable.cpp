/*
 * is_printable.cpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/


#include <test.h>
#include <iostream>

#include <is_printable.hpp>

struct NotPrintable {};

struct Printable{};
std::ostream& operator<<(std::ostream& os, Printable&) { return os; }

BOOST_AUTO_TEST_CASE(test)
{
	BOOST_CHECK(is_printable<int>::value);
	BOOST_CHECK(not is_printable<NotPrintable>::value);
	BOOST_CHECK(is_printable<Printable>::value);
}

