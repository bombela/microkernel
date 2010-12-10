/*
 * irq.h
 * Copyright © 2010 Alexandre Gau <gau.alexandre@gmail.com>
 *
*/

#pragma once
#ifndef IRQ_H
#define IRQ_H

namespace kernel
{

#define IRQ_SIZE	16

class Irq
{
	public:
		typedef void (*irq_handler_t)(int irq_number);

		Irq();
		~Irq();
		bool	setIrqHandler(int, irq_handler_t);

	private:

}; // class Irq

} // namespace kernel


#endif /* IRQ_H */
