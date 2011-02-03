/*
 * bind.cpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <test.h>
#include <kernel/std/bind>

namespace kstd = ::std::kernel_std;

int a__ = 44;

void lolita() { a__ = 2; }
void pouet(const int&) { a__ = 3; }
void kiki(int& v) { v += 3; }

struct Toto {
	mutable int _v;
	Toto(): _v(42) {}
	void job() { _v = 10; }
	void job2(int) const { _v = 12; }
	void hihi(int) {}
	void modif(int& m) { m += 5; }

	void operator()(float) { _v = 55; a__ = 133; }
};

BOOST_AUTO_TEST_CASE(impl_simple_fun)
{
	a__ = 4;
	kstd::bind_impl<void (*)(), void ()> f(&lolita);
	f();
	BOOST_CHECK(a__ == 2);
}

BOOST_AUTO_TEST_CASE(impl_simple_fun_const)
{
	a__ = 4;
	(kstd::bind_impl<void (*)(), void ()>(&lolita))();
	BOOST_CHECK(a__ == 2);
}

BOOST_AUTO_TEST_CASE(impl_simple_fun_constref)
{
	a__ = 4;
	(kstd::bind_impl<void (*)(const int&), void (const int&)>(&pouet))(22);
	BOOST_CHECK(a__ == 3);
}

BOOST_AUTO_TEST_CASE(impl_simple_fun_ref)
{
	int v = 4;
	(kstd::bind_impl<void (*)(int&), void (int&)>(&kiki))(v);
	BOOST_CHECK(v == 7);
}

BOOST_AUTO_TEST_CASE(impl_functor)
{
	a__ = 4;
	Toto t;
	(kstd::bind_impl<Toto, void (float)>(t))(22.f);
	BOOST_CHECK(a__ == 133);
}

BOOST_AUTO_TEST_CASE(impl_functor_by_ref)
{
	a__ = 4;
	Toto t;
	(kstd::bind_impl<kstd::ref_impl<Toto>, void (float)>(kstd::ref(t)))(22.f);
	BOOST_CHECK(t._v == 55);
	BOOST_CHECK(a__ == 133);
}
