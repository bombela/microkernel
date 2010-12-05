/*
 * pic_i8259.h
 * Copyright © 2010 Alexandre Gau <gau.alexandre@gmail.com>
 *
*/

#pragma once
#ifndef PIC_I8259_H
#define PIC_I8259_H


#include <kernel/types.h>

namespace driver {

class PIC_i8259 {
	public:
		PIC_i8259();
		~PIC_i8259() = default;

		void	release() const;
		bool	enable(uint8_t);
		bool	disable(uint8_t);

	private:
		void irq_init() const;
		
};

} // namespace driver

#endif /* PIC_I8259_H  */
