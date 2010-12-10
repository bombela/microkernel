/*
 * irq.cpp
 * Copyright © 2010 Alexandre Gau <gau.alexandre@gmail.com>
 *
*/


#include <kernel/irq.h>
#include <kernel/interrupt.h>
#include <kernel/pic_i8259.h>


extern uint32_t irq_wrapper[IRQ_SIZE];

namespace kernel
{
	extern Interrupt interrupt;

	extern "C" Irq::irq_handler_t		irq_handler[IRQ_SIZE] = { nullptr, };
	Irq::Irq()
	{
		driver::PIC_i8259::init();
	}

	Irq::~Irq()
	{
		driver::PIC_i8259::releaseAll();
	}

	bool	Irq::setIrqHandler(int NbIrq, irq_handler_t handler)
	{
		int ret;

		if (NbIrq < 0 || NbIrq > IRQ_SIZE)
			return false;

		irq_handler[NbIrq] = handler;
		ret = interrupt.setInterruptHandler(32 + NbIrq, (void*)irq_wrapper[NbIrq]);
		if (ret == -1)
			return false;
		else
			return true;
	}


} // namespace kernel
