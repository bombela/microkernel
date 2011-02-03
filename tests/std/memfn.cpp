/*
 * memfn.cpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <test.h>
#include <kernel/std/memfn>

namespace kstd = ::std::kernel_std;

struct Toto {
	mutable int _v;
	Toto(): _v(42) {}
	void job() { _v = 10; }
	void job2(int) const { _v = 12; }
	void hihi(int) {}
	void modif(int& m) { m += 5; }
};

BOOST_AUTO_TEST_CASE(basic_ref)
{
	kstd::mem_fn_impl_m<void (Toto::*)()> f(&Toto::job);

	Toto t;
	f(t);
	BOOST_CHECK(t._v == 10);
}

BOOST_AUTO_TEST_CASE(basic_ptr)
{
	kstd::mem_fn_impl_m<void (Toto::*)()> f(&Toto::job);

	Toto t;
	f(&t);
	BOOST_CHECK(t._v == 10);
}

BOOST_AUTO_TEST_CASE(basic_args)
{
	kstd::mem_fn_impl_m<void (Toto::*)(int)> f(&Toto::hihi);

	Toto t;
	f(t, 42);
}

BOOST_AUTO_TEST_CASE(basic_rvalue)
{
	kstd::mem_fn_impl_m<void (Toto::*)(int&)> f(&Toto::modif);

	Toto t;
	int a = 10;
	f(t, a);
	BOOST_CHECK(a == 15);
}

BOOST_AUTO_TEST_CASE(basic_const)
{
	kstd::mem_fn_impl_c<void (Toto::*)(int) const> f(&Toto::job2);

	Toto t;
    f(const_cast<const Toto&>(t), 2);
	BOOST_CHECK(t._v == 12);
}

BOOST_AUTO_TEST_CASE(helper_ref)
{
	Toto a;
	kstd::mem_fn(&Toto::job)(a);
	BOOST_CHECK(a._v == 10);
}

BOOST_AUTO_TEST_CASE(helper_ptr)
{
	Toto a;
	kstd::mem_fn(&Toto::job)(&a);
	BOOST_CHECK(a._v == 10);
}

BOOST_AUTO_TEST_CASE(helper_const)
{
	Toto a;
	kstd::mem_fn(&Toto::job2)(a, 42);
	BOOST_CHECK(a._v == 12);
	a._v = 2;
	kstd::mem_fn(&Toto::job2)(const_cast<const Toto*>(&a), 42);
	BOOST_CHECK(a._v == 12);
}
