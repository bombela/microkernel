/*
 * interrupt.h
 * Copyright © 2010 François-Régis 'fregux' Robert <robert.fregis@gmail.com>
 * Copyright © 2010 Alexandre Gau <gau.alexandre@gmail.com>
 *
*/

#ifndef __H_INTERRUPT__
#define __H_INTERRUPT__

#include <kernel/types.h>
#include <kernel/std/array.hpp>

#define IDT_SIZE 47

namespace kernel {
	struct IDT_Item {
		uint16_t Offset_Low;
		uint16_t Selecteur;
		uint16_t Type;
		uint16_t Offset_High;
	} __attribute__((packed));

	
	struct IDT_ptrIDTR {
		uint16_t Limite;
		void* Base;
	} __attribute__((packed));


	class Interrupt {
	public:
		Interrupt();
		~Interrupt();

		void	enableInterrupt();
		void	disableInterrupt();
		int		setInterruptHandler(uint16_t, void*);
		void*	getInterruptHandler(uint16_t);

	private:
		std::array<IDT_Item, IDT_SIZE> Idt;
	};
} /* namespace kernel */

#endif /* __H_INTERRUPT__ */
