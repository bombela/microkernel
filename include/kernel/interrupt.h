/*
 * interrupt.h
 * Copyright © 2011, François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <kernel/types.h>
#include <attributes.h>
#include <array>

namespace kernel {

class InterruptManager
{
	public:
		InterruptManager();
		~InterruptManager();

		InterruptManager(const InterruptManager& from) = default;
		InterruptManager(InterruptManager&& from) = default;
		InterruptManager& operator=(const InterruptManager& from) = default;
		InterruptManager& operator=(InterruptManager&& from) = default;

		inline void	setInts() { asm volatile ("sti"); }
		inline void	clsInts() { asm volatile ("cli"); }

	private:
		struct IDTEntry {
			uint16_t offset_low;
			uint16_t segment_selector;
	
			uint8_t reserved:5;
			uint8_t flags:3;

			enum Type { interrupt = 0, trap = 1 };
			uint8_t type:3;

			enum OpSize { op16 = 0, op32 = 1 };
			uint8_t op_size:1;

			uint8_t zero:1;
			uint8_t dpl:2;
			uint8_t present:1;

			uint16_t offset_high;

			inline void setOffset(uint32_t off) {
				offset_low  = static_cast<uint16_t>(off);
				offset_high = static_cast<uint16_t>(off >> 16);
			}
		} PACKED ALIGNED(8);

		std::array<IDTEntry, 256> _idt;
};

} // namespace kernel

#endif /* INTERRUPT_H */
