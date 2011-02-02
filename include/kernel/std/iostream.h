/* vim: ft=cpp
 * Local Variables:
 * mode:c++
 * End:
 * iostream
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef IOSTREAM_H
#define IOSTREAM_H

#include <kernel/std/ostream.h>

namespace kernel {
namespace std {

extern std::ostream cout;
extern const bool cout_initialized;

} // namespace std
} // namespace kernel

#endif /* IOSTREAM_H */
