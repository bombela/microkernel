/*
 * interrupt.h
 * Copyright © 2011, François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <kernel/types.h>
#include <kernel/segmentation.h>
#include <kernel/memory.h>
#include <attributes.h>
#include <array>
#include <function>

namespace kernel {
namespace interrupt {

enum Type { BITS = 3, taskgate = 5, interrupt = 6, trapgate = 7 };

struct Description
{
	uint16_t               offset_low;
	segmentation::Selector selector;

	uint8_t                reserved:5;
	uint8_t                flags:3;

	uint8_t                type:3; // Type
	uint8_t                op_size:1; // memory::OperationSize
	uint8_t                zero:1;
	uint8_t                privilege:2; // memory::Privilege
	bool                   present:1;

	uint16_t               offset_high;

	inline void setOffset(uint32_t off) {
		offset_low  = static_cast<uint16_t>(off);
		offset_high = static_cast<uint16_t>(off >> 16);
	}

	Description():
		flags(0),
		op_size(std::enum_cast(memory::OperationSize::mode32)),
		zero(0),
		present(false) {}
	
	Description(segmentation::Selector s,
			uint32_t off, Type t, memory::Privilege p):
		offset_low(off),
		selector(s),
		flags(0),
		type(std::enum_cast(t)),
		op_size(std::enum_cast(memory::OperationSize::mode32)),
		zero(0),
		privilege(std::enum_cast(p)),
		present(true),
		offset_high(off >> 16)
		{}
} PACKED ALIGNED(8);

extern "C" const uint32_t* const __b_load;

class Trampoline
{
	public:
		enum class ErrorCode { yes, no };

		Trampoline():
			_push1(0x6A),
			_zero(0),
			_push2(0x68),
			_int_idx(257),
			_jmp(0xE9),
			_function(0)
   		{}
		
		void buildCode(void (*f)(), uint8_t idx, ErrorCode ec)
		{
				_zero = (ec == ErrorCode::yes);
				_int_idx = idx;
				uint8_t* eip = reinterpret_cast<uint8_t*>(&this->_jmp) + 5;
				_function = reinterpret_cast<uint8_t*>(f) - eip;
		}

		uint32_t addr() const {
			if (_zero == 0)
				return reinterpret_cast<uint32_t>(this);
			return reinterpret_cast<uint32_t>(this) + 2;
		}
	private:
/*
 * 6A 00                push   $0
 * 68 AA AA AA AA       push   $INT_IDX
 * E9 CC CC CC CC       jmp    RELATIVE FUNCTION ADDR
 */
		uint8_t        _push1;
		uint8_t        _zero;
		uint8_t        _push2;
		uint32_t       _int_idx;
		uint8_t        _jmp;
		int32_t        _function;
} PACKED ALIGNED(16);

extern "C" void interrupt_wrapper();

class Manager
{
	public:
		typedef std::function<void (int, int)> handler_t;

		Manager();
		~Manager();

		Manager(const Manager& from) = default;
		Manager(Manager&& from) = default;
		Manager& operator=(const Manager& from) = default;
		Manager& operator=(Manager&& from) = default;

		inline void	enable() { asm volatile ("sti"); }
		inline void	disable() { asm volatile ("cli"); }

		void setHandler(uint8_t idx, const handler_t& h);

		// really ugly
		static Manager* _this;
		void interruptHandler(int intCode, int errCode);

	private:
		std::array<Description, 256> _idt;
		std::array<Trampoline,  256> _trampolines;
		std::array<handler_t,   256> _handlers;

		inline void buildTrampoline(uint8_t idx,
				Trampoline::ErrorCode ec)
		{
			_trampolines[idx].buildCode(interrupt_wrapper, idx, ec);
		}
};

} // namespace interrupt

extern interrupt::Manager interruptManager;

} // namespace kernel

#endif /* INTERRUPT_H */
