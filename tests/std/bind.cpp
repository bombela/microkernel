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
int tryret(int a) { return a * 2; }

struct Toto {
	mutable int _v;
	Toto(): _v(42) {}
	void job() { _v = 10; }
	void job2(int) const { _v = 12; }
	void hihi(int) {}
	void modif(int& m) { m += 5; }

	void operator()(float) const { _v = 55; a__ = 133; }
};

struct Titi {
	mutable int _v;
	Titi(): _v(42) {}

	void operator()(int v) const { _v = v; a__ = 154; }
};

BOOST_AUTO_TEST_CASE(impl_simple_fun)
{
	a__ = 4;
	kstd::bind_impl<void (*)()> f(&lolita);
	f();
	BOOST_CHECK(a__ == 2);
}

void match(int a, float b, int c)
{
	std::cout << a << ", " << b << ", " << c << std::endl;
	a__ = 99;
}

template <typename F>
void const_call(const F& f) {
	const_cast<const F&>(f)();
}

BOOST_AUTO_TEST_CASE(bind_impl_basic_test)
{
	a__ = 4;
	kstd::bind_impl<void (*)(int, float, int), int, float, int> s(&match, 2, 22.f, 3);
	s();
	BOOST_CHECK(a__ == 99);
	
	a__ = 4;
	const_call(s);
	BOOST_CHECK(a__ == 99);

}

BOOST_AUTO_TEST_CASE(bind_simple_func_one_arg)
{
	a__ = 5;
	kstd::bind_impl<void (*)(const int&), const int&> f(&pouet, 42);
	f();
	BOOST_CHECK(a__ == 3);
}

BOOST_AUTO_TEST_CASE(bind_simple_func_tryret)
{
	kstd::bind_impl<int (*)(int), int> f(&tryret, 21);
	int r = f();
	BOOST_CHECK(r == 42);
}

BOOST_AUTO_TEST_CASE(impl_simple_fun_constref)
{
	a__ = 4;
	(kstd::bind_impl<void (*)(const int&), const int&>(&pouet, 22))();
	BOOST_CHECK(a__ == 3);
}

BOOST_AUTO_TEST_CASE(impl_simple_fun_ref)
{
	int v = 4;
	(kstd::bind_impl<void (*)(int&), int&>(&kiki, v))();
	BOOST_CHECK(v == 7);
	v = 2;
	(kstd::bind_impl<void (*)(int&),
	 kstd::ref_wrapper<int>>(&kiki, kstd::ref(v)))();
	BOOST_CHECK(v == 5);
}

BOOST_AUTO_TEST_CASE(impl_functor)
{
	a__ = 4;
	Toto t;
	(kstd::bind_impl<Toto, float>(t, 22.f))();
	BOOST_CHECK(a__ == 133);
}

BOOST_AUTO_TEST_CASE(impl_functor_by_ref)
{
	a__ = 4;
	Titi f;
	(
	 kstd::bind_impl<kstd::ref_wrapper<Titi>, int>(kstd::ref(f), 98)
	 )();
	BOOST_CHECK(f._v == 98);
	BOOST_CHECK(a__ == 154);
}
