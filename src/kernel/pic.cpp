/*
 * pic_i8259.cpp
 * Copyright © 2010, Alexandre Gau <gau.alexandre@gmail.com>
 * Copyright © 2011, François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <kernel/pic.h>

#include KERNEL_PIC_DEBUG
#include KERNEL_PIC_CHECK

namespace kernel {
namespace pic {

Manager::Manager():
	_masterMask(OCW1_M), _slaveMask(OCW1_S)
{
	dbg("PIC i8259s master/slave initalization");

	kernel::io::out::byte(ICW1_M, PORT1_M);
    kernel::io::out::byte(ICW2_M, PORT2_M);
    kernel::io::out::byte(ICW3_M, PORT2_M);
    kernel::io::out::byte(ICW4_M, PORT2_M);
    kernel::io::out::byte(OCW1_M, PORT2_M);

    kernel::io::out::byte(ICW1_S, PORT1_S);
	kernel::io::out::byte(ICW2_S, PORT2_S);
	kernel::io::out::byte(ICW3_S, PORT2_S);
	kernel::io::out::byte(ICW4_S, PORT2_S);
    kernel::io::out::byte(OCW1_S, PORT2_S);
}

Manager::~Manager()
{
	dbg("PIC i8259s master/slave release");
	kernel::io::out::byte(0xFF, PORT2_M);
    kernel::io::out::byte(0xFF, PORT2_S);
}

} // namespace pic
} // namespace kernel
