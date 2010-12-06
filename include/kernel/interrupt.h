#ifndef __H_INTERRUPT__
#define __H_INTERRUPT__

#include <kernel/types.h>
#include <kernel/std/array.hpp>

#define IDT_Size 47

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
		int setInterrupt(uint16_t, void*);

	private:
		std::array<IDT_Item, IDT_Size> Idt;
		//IDT_Item Idt[IDT_Size];
	};
} /* namespace kernel */

#endif /* __H_INTERRUPT__ */
