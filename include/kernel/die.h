/*
 * die.h
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef DIE_H
#define DIE_H

#include <attributes.h>

namespace kernel {

NORETURN void die();
extern "C" NORETURN void __kernel_print_stop_msg();

} // namespace kernel

#endif /* DIE_H */
