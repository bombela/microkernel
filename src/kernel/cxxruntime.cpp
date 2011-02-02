/*
 * cxxruntime.cpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <stack.hpp>
#include <new>
#include <kernel/die.h>
#include <ostream>
#include <attributes.h>

#include KERNEL_CXXRUNTIME_DEBUG
#include KERNEL_CXXRUNTIME_CHECK

namespace std {
	using namespace kernel::std;
} // namespace std

// --- delete operator (used by virtual destructor for example)

void operator delete(UNUSED void* ptr) noexcept
{
	dbg("%", ptr);
}

// --- die on pure virtual call

extern "C" void __cxa_pure_virtual(void)
{
	// cout toussa... ("pure virtual method called\n");
	kernel::die();
}

// --- guard support (used for initialize static scoped object)

// it's a 64bits int, we can do what we want with it.
typedef uint64_t __guard;

// TODO would need to be really implemented in case of multi-threading support!
extern "C" int __cxa_guard_acquire(__guard*)
{
	return 0;
}

// TODO would need to be really implemented in case of multi-threading support!
extern "C" int __cxa_guard_release(__guard*)
{
	return 0;
}

// --- handle used by different translation unit to manage object construction.
// It's supported by the linker. The main unit have to be 0.
void* __dso_handle = 0;

// --- called to registed some function that have to be called at finalize time.

struct Destructor
{
	void (*dtor)(void*);
	void* obj;
	void destroy() { dtor(obj); }

	friend std::ostream& operator<<(std::ostream& os, const Destructor& d)
	{
		os << "Destructor/dtor_func=" << reinterpret_cast<void*>(d.dtor)
			<< ", obj=" << d.obj << "/";
		return os;
	}
};

namespace {

typedef std::stack<Destructor, 16> CleanupStack;
static char cleanupStack [sizeof (CleanupStack)];

} // namespace 

extern "C" int __cxa_atexit(void (*dtor)(void*), void* obj, const void*,
		UNUSED const void* dso)
{
	CleanupStack& cs = reinterpret_cast<CleanupStack&>(cleanupStack);
	
	assert(not cs.full());
	dbg("(dtor %, arg %, dso %)", dtor, obj, dso);
	cs.push({ dtor, obj });
	return 0;
}

void call_registereds_destructors()
{
	CleanupStack& cs = reinterpret_cast<CleanupStack&>(cleanupStack);

	dbg("stack size: %", cs.size());
	while (cs.size())
		cs.pop_get().destroy();
}

// --- access to the ctors section (who is simply an array of func_ptr)
extern "C" {
	typedef void (*func_ptr)();
	extern func_ptr __b_ctors[];
	extern func_ptr __e_ctors[];
}

namespace cxxruntime {

void call_static_constructors()
{
	dbg("globales initialization\n");

	for (func_ptr* f = __e_ctors - 1; f >= __b_ctors; --f)
		(*f)();
}

// --- public interface

void initialize()
{
	new (&cleanupStack) CleanupStack;
	call_static_constructors();
}

void finalize()
{
	call_registereds_destructors();
}

} // namespace cxxruntime
