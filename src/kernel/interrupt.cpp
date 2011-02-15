/*
 * interrupt.cpp
 * Copyright © 2011, François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <kernel/interrupt.h>
#include <iomanip>
#include <kernel/console.h>
#include <kernel/vga_console.h>
#include <attributes.h>

#include KERNEL_INTERRUPT_DEBUG
#include KERNEL_INTERRUPT_CHECK

namespace kernel {
namespace interrupt {

namespace documentation {

enum class Type      { fault, trap, abort, interrupt };
enum class ErrorCode { yes, zero, no };

struct ExceptionDoc {

	uint8_t     idx;
	const char* mnemnonic;
	const char* desc;
	Type        type;
	ErrorCode   errorCode;
	const char* source;

	friend std::ostream& operator<<(std::ostream& os, const ExceptionDoc& e)
	{
		static const char* const types[] = {
			"fault", "trap", "abort", "interrupt"
		};
		static const char* const errors[] = {
			"yes", "zero", "no"
		};
		os(R"(interrup #%c "%" "%" type="%" errcode="%" src="%")",
				e.idx,
				e.mnemnonic,
				e.desc,
				types[static_cast<int>(e.type)],
				errors[static_cast<int>(e.errorCode)],
				e.source
				);
		return os;		
	}
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
	32 /* .. 256 */, "-", "Not an exception. User defined interrupt.",
		Type::fault, ErrorCode::no,
		"User defined interrupt." 
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

extern "C" void interrupt_wrapper();

void Manager::setHandler(uint8_t idx, const handler_t& h) {
	_trampolines[idx].buildCode(interrupt_wrapper, idx,
			(documentation::get(idx).errorCode == documentation::ErrorCode::no)
			? Trampoline::ErrorCode::no
			: Trampoline::ErrorCode::yes
			);
	_idt[idx] = Description(
			segmentation::Manager::buildSelector(
				memory::Privilege::kernel, segmentation::Manager::Segment::kernel_code
				),
			_trampolines[idx].addr(), Type::interrupt, memory::Privilege::kernel
			);
	_handlers[idx] = h;
}

void toto(int a, int b) {
	std::cout("%-%", a, b);
}

Manager::Manager()
{
	dbg("starting");
	disable();

	_this = this;

	// setup default handler
	for (size_t i = 0; i < _idt.size(); ++i)
		setHandler(i, handler_t( [](int idx, int ec) {
					auto e = documentation::get(idx);
					if (e.type == documentation::Type::abort)
						std::cout << std::color::red;
					else
						std::cout << std::color::yellow;
					std::cout
						<< e << " errval=" << ec
						<< std::color::ltgray
						<< std::endl;
					switch (e.type) {
						case documentation::Type::abort:
						case documentation::Type::fault:
						case documentation::Type::trap:
							die();
						case documentation::Type::interrupt:
							;
							}
				}));
	
	// load IDT
	struct {
		uint16_t idt_size;
		void*    idt_addr;
	} PACKED ptrIDTR{
		static_cast<uint16_t>(_idt.size() / sizeof _idt[0] - 1), &_idt[0]
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

inline void Manager::interruptHandler(int intCode, int errCode)
{
	_handlers[intCode](intCode, errCode);
}

Manager* Manager::_this = 0;
extern "C" void _interrupt_handler(int intCode, int errCode)
{
	Manager::_this->interruptHandler(intCode, errCode);
}

} // namespace interrupt
} // namespace kernel
