/*
 * cxxruntime.cpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <array.hpp>
#include <new>

#include KERNEL_CXXRUNTIME_DEBUG
#include KERNEL_CXXRUNTIME_CHECK

// support for static inside function

#if 0

// TODO
// it's a 64bits int, we can do what we want with it.
typedef uint64_t __guard;

// TODO implement fully
/*extern "C" void __cxa_guard_acquire(__guard*)
{
	// acquire mutex
}*/

// TODO implement fully
/*extern "C" void __cxa_guard_release(__guard*)
{
	// release mutex
}*/

// support for global initalisatoin
//void* __dso_handle = 0;

typedef void (*func_ptr)();
func_ptr cleanup[20] = { 0 };

void* cleanup_self[20];

/*extern "C" int __cxa_atexit(void (*func)(), void* self)
{
	console.write("__cxa_atexit\n");
  
	typedef void (*func_ptr)();
	func_ptr* fend = cleanup;
	int i = 0;
	while (*fend)
	{
		++fend;
		++i;
	}
	*fend = func;
	*++fend = 0;
	cleanup_self[i] = self;
	return 0;
}*/

void cleanglobal
{
	console.write("call destructor\n");
	func_ptr* i = cleanup;
	int j = 0;
	while (*i)
	{
		++i;
		++j;
	}
	while (--j, --i >= cleanup)
		((void (*)(void*))(*i)) (cleanup_self[j]);
}

#endif

// handle used by different translation unit to manage object construction.
// It's supported by the linker. The main unit have be 0.
void* __dso_handle = 0;

extern "C" int __cxa_atexit (void (*dtor)(), void* arg, const void* dso)
{
	dbg("(dtor %, arg %, dso %)", dtor, arg, dso);
  
	/*
	typedef void (*func_ptr)();
	func_ptr* fend = cleanup;
	int i = 0;
	while (*fend)
	{
		++fend;
		++i;
	}
	*fend = func;
	*++fend = 0;
	cleanup_self[i] = self;*/
	return 0;
}


extern "C" {
	typedef void (*func_ptr)();
	extern func_ptr __b_ctors[];
	extern func_ptr __e_ctors[];
}

namespace cxxruntime {

void call_static_constructors()
{
	dbg("call constructors\n");
	for (func_ptr* i = __e_ctors - 1; i >= __b_ctors; --i)
		(*i)();
}

} // namespace cxxruntime
