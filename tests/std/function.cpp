/*
 * function.cpp
 * Copyright © 2011, François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <test.h>
#include <kernel/std/function>
#include <kernel/std/bind>

namespace kstd = ::std::kernel_std;

BOOST_AUTO_TEST_CASE(fct_basic)
{
	kstd::function<int (char, float)> f;
	struct { char a[kstd::details::minsize]; void* b; } a;
	BOOST_CHECK(sizeof f == sizeof a);
}

int adder(char a, float b) { return a + b; }
int adder2(char a, float b) { return a + b + 1; }

BOOST_AUTO_TEST_CASE(fct_set)
{
	kstd::function<int (char, float)> f(&adder);
	BOOST_CHECK(f(1, 2) == 3);

	f = &adder2;
	BOOST_CHECK(f(1, 2) == 4);
}

struct Lolita { int adder(char a, float b) const { return a + b; } };

BOOST_AUTO_TEST_CASE(fct_functor)
{
	kstd::function<int (char, float)> f(kstd::bind(&adder, _1, _2));
	BOOST_CHECK(f(1, 2) == 3);
	
	f = kstd::bind(&adder2, _2, _1);
	BOOST_CHECK(f(1, 2) == 4);
	
	f = kstd::bind(&Lolita::adder, Lolita(), _1, _2);
	BOOST_CHECK(f(2, 3) == 5);
}

struct Big {
	void set(char c) { for(auto& i : data) i = c; }
	void check(char c) const { for(auto i : data) BOOST_REQUIRE(i == c); }
	char data[1024];
};

BOOST_AUTO_TEST_CASE(fct_big_functor)
{
	Big one;
	kstd::function<void (char), 1024 + 64> f;
	
	one.set('a');
	one.check('a');
	f = kstd::bind(&Big::set, &one, _1);
	f('b');
	one.check('b');
	
	f = kstd::bind(&Big::set, kstd::ref(one), _1);
	f('c');
	f = kstd::bind(&Big::check, kstd::cref(one), _1);
	f('c');
	f = kstd::bind(&Big::check, one, _1); // copy one
	one.set('a');
	f('c');
}

BOOST_AUTO_TEST_CASE(fct_value)
{
	kstd::function<int (char, float)> f(&adder);
	const char  a = 1;
	const float b = 2;
	BOOST_CHECK(f(a, b) == 3);

	f = &adder2;
	float c = 2;
	BOOST_CHECK(f(1, c) == 4);
}
