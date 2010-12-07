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
		static void		init();
		static void		releaseAll();
		static bool		enable(uint8_t);
		static bool		disable(uint8_t);
		static uint8_t	intToIrq(uint8_t);

	private:
		static uint8_t		NbToBit(uint8_t Nb);


};

} // namespace driver

#endif /* PIC_I8259_H  */
