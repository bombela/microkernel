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

	void operator()(float) { _v = 55; }
};

BOOST_AUTO_TEST_CASE(simple_fun)
{
	kstd::bind_impl<void (*)(), void ()> f(&lolita);
//	f();
	BOOST_CHECK(a__ == 2);
}
