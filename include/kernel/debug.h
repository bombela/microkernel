/*
 * debug.h
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef DEBUG_H
#define DEBUG_H

//#include <kernel/std/iostream>
//#include <kernel/std/formatter.hpp>

namespace kernel {
namespace debug {

template <typename... Args>
void printf(const char* fmt, Args...)
{
	//formatter::print()
}

} // namespace debug
} // namespace kernel

#endif /* DEBUG_H */
