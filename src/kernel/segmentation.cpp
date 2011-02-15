/*
 * segmentation.cpp
 * Copyright © 2011, François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <kernel/segmentation.h>
#include <iomanip>

#include KERNEL_SEGMENTATION_DEBUG
#include KERNEL_SEGMENTATION_CHECK

namespace kernel {
namespace segmentation {

Manager::Manager()
{
	dbg("starting");

	// flat model
	(*this)[Segment::null]
		= Descriptor();

	(*this)[Segment::kernel_code]
		= Descriptor(memory::Privilege::kernel, SegmentType::code);

	(*this)[Segment::kernel_data]
		= Descriptor(memory::Privilege::kernel, SegmentType::data);

	(*this)[Segment::userland_code]
		= Descriptor(memory::Privilege::userland, SegmentType::code);

	(*this)[Segment::userland_data]
		= Descriptor(memory::Privilege::userland, SegmentType::data);
	
	struct {
		uint16_t maxoffset;
		uint32_t base_addr;
	} PACKED ALIGNED(4) gdt_reg {
		static_cast<uint16_t>(_gdt.size() * sizeof _gdt[0] - 1),
		reinterpret_cast<uint32_t>(&_gdt[0])
	};

	uint16_t data_sel = buildSelector(memory::Privilege::kernel,
			Segment::kernel_data);

	asm volatile (R"ASM(
			lgdt %0
			/* jump to clean the CPU prefetch queue */
			ljmpl $0x8, $1f /* 0x8 == kernel_code segment */
			1:
			movw %%ax, %%ss
			movw %%ax, %%ds
			movw %%ax, %%es
			movw %%ax, %%fs
			movw %%ax, %%gs
			)ASM"
			::
			"m"  (gdt_reg),
			"ax" (data_sel)
			:"memory");
	dbg("started");
}

Manager::~Manager()
{
	dbg("your should never destroy the segmentation::Manager, "
				"you are now in indeterminate state");
}

} // namespace segmentation
} // namespace kernel
