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
	int adder(int a, int b) const { return a + b; }
};

struct Titi {
   	virtual int adder(int, int)	const = 0;
};

// from memfn_noinline_test.cpp
extern const Titi& getTata();

// ---- store PMF version

BOOST_AUTO_TEST_CASE(basic_ref)
{
	kstd::mem_fn_storepmf::mem_fn_impl<void (Toto::*)()> f(&Toto::job);

	Toto t;
	f(t);
	BOOST_CHECK(t._v == 10);
}

BOOST_AUTO_TEST_CASE(basic_ptr)
{
	kstd::mem_fn_storepmf::mem_fn_impl<void (Toto::*)()> f(&Toto::job);

	Toto t;
	f(&t);
	BOOST_CHECK(t._v == 10);
}

BOOST_AUTO_TEST_CASE(basic_args)
{
	kstd::mem_fn_storepmf::mem_fn_impl<void (Toto::*)(int)> f(&Toto::hihi);

	Toto t;
	f(t, 42);
}

BOOST_AUTO_TEST_CASE(basic_rvalue)
{
	kstd::mem_fn_storepmf::mem_fn_impl<void (Toto::*)(int&)> f(&Toto::modif);

	Toto t;
	int a = 10;
	f(t, a);
	BOOST_CHECK(a == 15);
}

BOOST_AUTO_TEST_CASE(basic_const)
{
	kstd::mem_fn_storepmf::mem_fn_impl<void (Toto::*)(int) const> f(&Toto::job2);

	Toto t;
    f(const_cast<const Toto&>(t), 2);
	BOOST_CHECK(t._v == 12);
}

BOOST_AUTO_TEST_CASE(helper_ref)
{
	Toto a;
	kstd::mem_fn_storepmf::storepmf_mem_fn(&Toto::job)(a);
	BOOST_CHECK(a._v == 10);
}

BOOST_AUTO_TEST_CASE(helper_ptr)
{
	Toto a;
	kstd::mem_fn_storepmf::storepmf_mem_fn(&Toto::job)(&a);
	BOOST_CHECK(a._v == 10);
}

BOOST_AUTO_TEST_CASE(helper_const)
{
	Toto a;
	kstd::mem_fn_storepmf::storepmf_mem_fn(&Toto::job2)(a, 42);
	BOOST_CHECK(a._v == 12);
	a._v = 2;
	kstd::mem_fn_storepmf::storepmf_mem_fn(&Toto::job2)
		(const_cast<const Toto*>(&a), 42);
	BOOST_CHECK(a._v == 12);
}

BOOST_AUTO_TEST_CASE(helper_adder)
{
	int r = kstd::storepmf_mem_fn(&Toto::adder)(Toto(), 1, 2);
	BOOST_CHECK(r == 3);
	
	Toto a;
	r = kstd::storepmf_mem_fn(&Toto::adder)(a, 1, 2);
	BOOST_CHECK(r == 3);
}

__attribute__ ((noinline))
AUTO_FUN( storepmf_test_virtual_build_memfn() )
(
 kstd::storepmf_mem_fn(&Titi::adder)
)

__attribute__ ((noinline)) int storepmf_test_virtual_impl()
{
	auto f = storepmf_test_virtual_build_memfn();
	return f(getTata(), 1, 2);
}

BOOST_AUTO_TEST_CASE(storepmf_test_virtual)
{
	int r = storepmf_test_virtual_impl();
	BOOST_CHECK(r == 3);
}

// ---- static version

BOOST_AUTO_TEST_CASE(static_sizecheck)
{
	BOOST_CHECK(sizeof kstd::static_mem_fn(&Toto::job) == 1);
}

BOOST_AUTO_TEST_CASE(static_helper_ref)
{
	Toto a;
	kstd::static_mem_fn(&Toto::job)(a);
	BOOST_CHECK(a._v == 10);
}

BOOST_AUTO_TEST_CASE(static_helper_ptr)
{
	Toto a;
	kstd::static_mem_fn(&Toto::job)(&a);
	BOOST_CHECK(a._v == 10);
}

BOOST_AUTO_TEST_CASE(static_helper_const)
{
	Toto a;
	kstd::static_mem_fn(&Toto::job2)(&a, 42);
	BOOST_CHECK(a._v == 12);
	a._v = 2;
	kstd::static_mem_fn(&Toto::job2)(a, 42);
	BOOST_CHECK(a._v == 12);
	a._v = 2;
	kstd::static_mem_fn(&Toto::job2)(const_cast<const Toto*>(&a), 42);
	BOOST_CHECK(a._v == 12);
}

BOOST_AUTO_TEST_CASE(static_adder)
{
	int r = kstd::static_mem_fn(&Toto::adder)(Toto(), 1, 2);
	BOOST_CHECK(r == 3);
	
	Toto a;
	r = kstd::static_mem_fn(&Toto::adder)(a, 1, 2);
	BOOST_CHECK(r == 3);
}

__attribute__ ((noinline))
AUTO_FUN( static_test_virtual_build_memfn() )
(
 kstd::static_mem_fn(&Titi::adder)
)

__attribute__ ((noinline)) int static_test_virtual_impl()
{
	auto f = static_test_virtual_build_memfn();
	return f(getTata(), 1, 2);
}

BOOST_AUTO_TEST_CASE(static_test_virtual)
{
	int r = static_test_virtual_impl();
	BOOST_CHECK(r == 3);
}

// ---- prefered version
//
BOOST_AUTO_TEST_CASE(prefered_version_test)
{
	int r = MEM_FN(&Toto::adder)(Toto(), 1, 2);
	BOOST_CHECK(r == 3);
}
