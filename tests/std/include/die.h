/*
 * die.h
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef DIE_H
#define DIE_H

namespace kernel {

void die() {
	extern void abort();
	::abort();
}

} // namespace kernel

#endif /* DIE_H */
