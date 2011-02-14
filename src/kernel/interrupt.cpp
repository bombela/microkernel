/*
 * interrupt.cpp
 * Copyright © 2011, François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <kernel/interrupt.h>

#include <kernel/console.h>
#include <kernel/vga_console.h>
#include <attributes.h>

#include KERNEL_INTERRUPT_DEBUG
#include KERNEL_INTERRUPT_CHECK

namespace kernel {
namespace interrupt {

namespace documentation {

enum class Type      { fault, trap, interrupt, abort };
enum class ErrorCode { yes, zero, no };

struct ExceptionDoc {

	uint8_t     idx;
	const char* mnemnonic;
	const char* desc;
	Type        type;
	ErrorCode   errorCode;
	const char* source;

} const exceptionsDoc[] = {

	{  0, "#DE", "Divide Error",
		Type::fault, ErrorCode::no,
		"DIV and IDIV instructions."
	},
	{  1, "#DB", "RESERVED",
		Type::fault, ErrorCode::no,
		"For Intel use only." 
	},
	{  2, "-", "NMI Interrupt",
		Type::interrupt, ErrorCode::no,
		"Nonmaskable external interrupt." 
	},
	{  3, "#BP", "Breakpoint",
		Type::trap, ErrorCode::no,
		"INT 3 instruction." 
	},
	{  4, "#OF", "Overflow",
		Type::trap, ErrorCode::no,
		"INTO instruction." 
	},
	{  5, "#BR", "BOUND Range Exceeded",
		Type::fault, ErrorCode::no,
		"BOUND instruction." 
	},
	{  6, "#UD", "Invalid Opcode (Undefined Opcode)",
		Type::fault, ErrorCode::no,
		"UD2 instruction or reserved opcode." 
	},
	{  7, "#NM", "Device Not Available (No Math Coprocessor)",
		Type::fault, ErrorCode::no,
		"Floating-point or WAIT/FWAIT instruction." 
	},
	{  8, "#DF", "Double Fault",
		Type::abort, ErrorCode::zero,
		"Any instruction that can generate an exception, an NMI, or an INTR." 
	},
	{  9, "-", "Coprocessor Segment Overrun (reserved)",
		Type::fault, ErrorCode::no,
		"Floating-point instruction." 
	},
	{ 10, "#TS", "Invalid TSS",
		Type::fault, ErrorCode::yes,
		"Task switch or TSS access." 
	},
	{ 11, "#NP", "Segment Not Present",
		Type::fault, ErrorCode::yes,
		"Loading segment registers or accessing system segments." 
	},
	{ 12, "#SS", "Stack-Segment Fault",
		Type::fault, ErrorCode::yes,
		"Stack operations and SS register loads." 
	},
	{ 13, "#GP", "General Protection",
		Type::fault, ErrorCode::yes,
		"Any memory reference and other protection checks." 
	},
	{ 14, "#PF", "Page Fault",
		Type::fault, ErrorCode::yes,
		"Any memory reference." 
	},
	{ 15, "-", "Intel reserved. Do not use.",
		Type::fault, ErrorCode::no,
		"Intel reserved. Do not use." 
	},
	{ 16, "#MF", "x87 FPU Floating-Point Error (Math Fault)",
		Type::fault, ErrorCode::no,
		"x87 FPU floating-point or WAIT/FWAIT instruction." 
	},
	{ 17, "#AC", "Alignment Check",
		Type::fault, ErrorCode::yes,
		"Any data reference in (zero, memory.3)" 
	},
	{ 18, "#MC", "Machine Check",
		Type::abort, ErrorCode::no,
		"Error codes (if any) and source are model dependent." 
	},
	{ 19, "#XM", "SIMD Floating-Point Exception",
		Type::fault, ErrorCode::no,
		"SSE/SSE2/SSE3 floating-point instructions" 
	}
};

const ExceptionDoc exceptionReserveddoc = {
	20 /* .. 31 */, "-", "Intel reserved. Do not use.",
		Type::fault, ErrorCode::no,
		"Intel reserved. Do not use." 
	};

const ExceptionDoc exceptionDoNotKnowdoc = {
	32 /* .. 256 */, "-", "Not a valid exception.",
		Type::fault, ErrorCode::no,
		"Not a valid exception." 
	};

ExceptionDoc get(unsigned idx) {
	if (idx <= 19)
		return exceptionsDoc[idx];
	auto r = exceptionDoNotKnowdoc;
	if (idx <= 31)
		r = exceptionReserveddoc;
	r.idx = idx;
	return r;
}

} // namespace documentation

Manager::Manager()
{
	dbg("starting");

	struct {
		uint16_t idt_size;
		void*    idt_addr;
	} PACKED ptrIDTR{
		static_cast<uint16_t>(_idt.size() / sizeof _idt[0]), &_idt[0]
	};

	asm volatile ("lidtl %0" :: "m" (ptrIDTR));
	enable();
	dbg("started");
}

Manager::~Manager()
{
	dbg("stopping");
	disable();
	dbg("stopped");
}

} // namespace interrupt
} // namespace kernel
