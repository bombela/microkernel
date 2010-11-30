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

NORETURN void die() {
	asm(R("
		cli
	1:
		hlt
		jmp 1b
	"))
}

} // namespace kernel

#endif /* DIE_H */
