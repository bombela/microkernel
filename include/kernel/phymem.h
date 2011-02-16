/*
 * phymem.h
 * Copyright © 2011, François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef PHYMEM_H
#define PHYMEM_H

#include <kernel/types.h>

#include KERNEL_PHYMEM_DEBUG
#include KERNEL_PHYMEM_CHECK

namespace kernel {
namespace phymem {

class Manager
{
	public:
		void init(unsigned mem_lower_kb, unsigned mem_upper_kb);
	private:
};

} // namespace phymem
} // namespace kernel

#include <check_off.h>
#include <debug_off.h>

#endif /* PHYMEM_H */
