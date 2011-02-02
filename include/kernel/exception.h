/*
 * exception.h
 * Copyright © 2010 Alexandre Gau <gau.alexandre@gmail.com>
 *
*/


#pragma once
#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <array>

#define EXCEPT_DIVIDE_ERROR                  0         // No error code
#define EXCEPT_DEBUG                         1         // No error code
#define EXCEPT_NMI_INTERRUPT                 2         // No error code
#define EXCEPT_BREAKPOINT                    3         // No error code
#define EXCEPT_OVERFLOW                      4         // No error code
#define EXCEPT_BOUND_RANGE_EXCEDEED          5         // No error code
#define EXCEPT_INVALID_OPCODE                6         // No error code
#define EXCEPT_DEVICE_NOT_AVAILABLE          7         // No error code
#define EXCEPT_DOUBLE_FAULT                  8         // Yes (Zero)
#define EXCEPT_COPROCESSOR_SEGMENT_OVERRUN   9         // No error code
#define EXCEPT_INVALID_TSS                  10         // Yes
#define EXCEPT_SEGMENT_NOT_PRESENT          11         // Yes
#define EXCEPT_STACK_SEGMENT_FAULT          12         // Yes
#define EXCEPT_GENERAL_PROTECTION           13         // Yes
#define EXCEPT_PAGE_FAULT                   14         // Yes
#define EXCEPT_INTEL_RESERVED_1             15         // No
#define EXCEPT_FLOATING_POINT_ERROR         16         // No
#define EXCEPT_ALIGNEMENT_CHECK             17         // Yes (Zero)
#define EXCEPT_MACHINE_CHECK                18         // No
#define EXCEPT_INTEL_RESERVED_2             19         // No
#define EXCEPT_INTEL_RESERVED_3             20         // No
#define EXCEPT_INTEL_RESERVED_4             21         // No
#define EXCEPT_INTEL_RESERVED_5             22         // No
#define EXCEPT_INTEL_RESERVED_6             23         // No
#define EXCEPT_INTEL_RESERVED_7             24         // No
#define EXCEPT_INTEL_RESERVED_8             25         // No
#define EXCEPT_INTEL_RESERVED_9             26         // No
#define EXCEPT_INTEL_RESERVED_10            27         // No
#define EXCEPT_INTEL_RESERVED_11            28         // No
#define EXCEPT_INTEL_RESERVED_12            29         // No
#define EXCEPT_INTEL_RESERVED_13            30         // No
#define EXCEPT_INTEL_RESERVED_14            31         // No

#define EXCEPT_SIZE	32

namespace kernel
{


class Exception{

	public:

		typedef void (*exception_handler_t)(int exception_number);

		Exception();
		~Exception() = default;

		bool	setExceptionHandler(int, exception_handler_t);

	private:
}; // class Exception

} // namespace kernel
#endif /* EXCEPTION_H */
