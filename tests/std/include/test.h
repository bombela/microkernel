/*
 * test.h
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef TEST_H
#define TEST_H

#define BOOST_TEST_MODULE kern0xstd
#include <boost/test/unit_test.hpp>

#define UNUSED __attribute__ ((unused))

// hack for keep alive BOOST_ASSERT!
#include <stdlib.h>
#include <debug.h>
#include <basename.h>
#undef BOOST_ASSERT
#define BOOST_ASSERT(__expr) do { if (not (__expr)) { \
	kernel::debug::printf(__FILE__ ":% % - /!\\ ASSERTION ERROR /!\\ (" \
			#__expr ")\n" + utility::basename(__FILE__), \
			__LINE__, __PRETTY_FUNCTION__); abort(); } } while (0)

#endif /* TEST_H */
