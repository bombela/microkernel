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
	int _v;
	Titi(): _v(42) {}

	void operator()(int v) { _v = v; a__ = 154; }
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
	a__ = 99 + a + b + c;
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
	BOOST_CHECK(a__ == 126);
	
	a__ = 4;
	const_call(s);
	BOOST_CHECK(a__ == 126);

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

BOOST_AUTO_TEST_CASE(bind_simple_const_arg)
{
	const int v = 21;
	kstd::bind_impl<int (*)(int), int> f(&tryret, v);
	int r = f();
	BOOST_CHECK(r == 42);
}

BOOST_AUTO_TEST_CASE(impl_simple_fun_constref)
{
	a__ = 4;
	(kstd::bind_impl<void (*)(const int&), const int&>(&pouet, 22))();
	BOOST_CHECK(a__ == 3);
}
/*
BOOST_AUTO_TEST_CASE(impl_simple_fun_ref)
{
	int v = 4;
	(kstd::bind_impl<void (*)(int&),
	 kstd::ref_wrapper<int>>(&kiki, kstd::ref(v)))();
	BOOST_CHECK(v == 5);
}
*/
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

struct Tata {
	Tata() {}
	~Tata() {}

	Tata(const Tata&) { a__ += 5; }
	Tata(Tata&&) { a__ += 10; }
	Tata& operator=(const Tata&) { return *this; }

	void operator()(int v) const { a__ += v; }
};

BOOST_AUTO_TEST_CASE(check_movector_usage)
{
	a__ = 14;
	(
	 kstd::bind_impl<Tata, int>(Tata(), 101)
	)();
	BOOST_CHECK(a__ == (14 + 10 + 101));

	Tata t;
	a__ = 14;
	(
	 kstd::bind_impl<Tata, int>(t, 101)
	)();
	BOOST_CHECK(a__ == (14 + 5 + 101));
}

BOOST_AUTO_TEST_CASE(bind_impl_lvalue_scalar)
{
	a__ = 4;
	const int a = 1;
	const float b = 2;
	const int c = 3;
	kstd::bind_impl<void (*)(int, float, int), int, float, int>
		s(&match, a, b, c);
	s();
	BOOST_CHECK(a__ == 105);
	
	a__ = 4;
	const_call(s);
	BOOST_CHECK(a__ == 105);
}

struct A { A() {} };
struct B { B() {} };

static const A testA;
static const B testB;

void lvaluetest(A, B) { a__ = 66; }

BOOST_AUTO_TEST_CASE(bind_impl_lvalue_obj)
{
	a__ = 4;
	kstd::bind_impl<void (*)(A, B), A, B>
		s(&lvaluetest, testA, testB);
	s();
	BOOST_CHECK(a__ == 66);
	
	a__ = 4;
	const_call(s);
	BOOST_CHECK(a__ == 66);
}

BOOST_AUTO_TEST_CASE(placeholder_basic)
{
	kstd::bind_impl<int (*)(int), decltype(_1)> f(&tryret, _1);
	int r = f(22);
	BOOST_CHECK(r == 44);
}

int adder(int a, char b)
{
	return a + b;
}

BOOST_AUTO_TEST_CASE(placeholder_bind2st)
{
	kstd::bind_impl<int (*)(int, char), int, char> f(&adder, 255, 127);
	int r = f();
	BOOST_CHECK(r == 255 + 127);
	
	kstd::bind_impl<int (*)(int, char), int, decltype(_1)>
		f2(&adder, 255, _1);
	int r2 = f2(127);
	BOOST_CHECK(r2 == 255 + 127);
}

BOOST_AUTO_TEST_CASE(placeholder_bind1st)
{
	kstd::bind_impl<int (*)(int, char), decltype(_1), char>
		f(&adder, _1, 127);
	int r = f(255);
	BOOST_CHECK(r == 255 + 127);
}

BOOST_AUTO_TEST_CASE(placeholder_nonconst)
{
	kstd::placeholder::arg<1> my1;
	kstd::bind_impl<int (*)(int, char), decltype(my1), char>
		f(&adder, my1, 127);
	int r = f(255);
	BOOST_CHECK(r == 255 + 127);
}

BOOST_AUTO_TEST_CASE(placeholder_call_lvalue)
{
	kstd::bind_impl<int (*)(int, char), decltype(_1), char>
		f(&adder, _1, 127);
	int a = 255;
	int r = f(a);
	BOOST_CHECK(r == 255 + 127);
	const int b = 256;
	r = f(b);
	BOOST_CHECK(r == 256 + 127);
}

void tryreference(int& o, int a, int b) { o = a + b; }
void tryreference2(int a, int b, int& o) { o = a + b; }

BOOST_AUTO_TEST_CASE(placeholder_call_ref)
{
	kstd::bind_impl<void (*)(int&, int, int), decltype(_1), int, int>
		f(&tryreference, _1, 4, 6);
	int r = 0;
	f(r);
	BOOST_CHECK(r == 10);
}

BOOST_AUTO_TEST_CASE(placeholder_call_ref2)
{
	kstd::bind_impl<void (*)(int, int, int&), int, int, decltype(_1)>
		f(&tryreference2, 4, 6, _1);
	int r = 0;
	f(r);
	BOOST_CHECK(r == 10);
}

int bigadder(float, int a, char b)
{
	return a + b;
}

BOOST_AUTO_TEST_CASE(placeholder_bind2args)
{
	kstd::bind_impl<int (*)(float, int, char), float,
		decltype(_1), decltype(_2)> f(&bigadder, 22.f, _1, _2);

	int r = f(255, 127);
	BOOST_CHECK(r == 255 + 127);
	
	int a = 255;
	const int b = 127;
	r = f(a, b);
	BOOST_CHECK(r == 255 + 127);
}

BOOST_AUTO_TEST_CASE(placeholder_bind2args_reverse)
{
	kstd::bind_impl<int (*)(float, int, char),
		decltype(_1), decltype(_2), char> f(&bigadder, _1, _2, 127);

	int r = f(22.f, 255);
	BOOST_CHECK(r == 255 + 127);
	
	float a = 22.f;
	const int b = 255;
	r = f(a, b);
	BOOST_CHECK(r == 255 + 127);
}

BOOST_AUTO_TEST_CASE(placeholder_bind2args_middle)
{
	kstd::bind_impl<int (*)(float, int, char),
		decltype(_1), int, decltype(_2)> f(&bigadder, _1, 255, _2);

	int r = f(22.f, 127);
	BOOST_CHECK(r == 255 + 127);
	
	float a = 22.f;
	const int b = 127;
	r = f(a, b);
	BOOST_CHECK(r == 255 + 127);
}

int sub(int a, int b) { return a - b; }

BOOST_AUTO_TEST_CASE(placeholder_bind_xchang)
{
	kstd::bind_impl<int (*)(int, int),
		decltype(_1), decltype(_2)> f(&sub, _1, _2);

	int r = f(10, 5);
	BOOST_CHECK(r == 5);
	
	kstd::bind_impl<int (*)(int, int),
		decltype(_2), decltype(_1)> g(&sub, _2, _1);

	r = g(10, 5);
	BOOST_CHECK(r == -5);
}

int extrem(int a, int b, int c, int d, int e,
		int f, int g, int h, int i, int j, int k)
{
	return a + b + c + d + e + f + g + h + i + j + k;
}

BOOST_AUTO_TEST_CASE(placeholder_bind_extrem)
{
	kstd::bind_impl<
		int (*)(int, int, int, int, int, int, int, int, int, int, int),
			decltype(_1),
			decltype(_2),
			decltype(_3),
			decltype(_4),
			decltype(_5),
			int,
			decltype(_6),
			decltype(_7),
			decltype(_8),
			decltype(_9),
			kstd::placeholder::arg<10>
				> f(&extrem, _1, _2, _3, _4, _5, 42, _6, _7, _8, _9,
						kstd::placeholder::arg<10>());

	int r = f(1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
	BOOST_CHECK(r == 52);
	int a = 2;
	r = f(1, 1, 1, 1, a, 1, 1, 1, 1, 1);
	BOOST_CHECK(r == 53);
}
