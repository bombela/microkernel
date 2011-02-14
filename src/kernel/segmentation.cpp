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
		= Descriptor(Privilege::kernel, SegmentType::code);

	(*this)[Segment::kernel_data]
		= Descriptor(Privilege::kernel, SegmentType::data);

	(*this)[Segment::userland_code]
		= Descriptor(Privilege::userland, SegmentType::code);

	(*this)[Segment::userland_data]
		= Descriptor(Privilege::userland, SegmentType::data);
	
	struct {
		uint16_t maxoffset;
		uint32_t base_addr;
	} PACKED ALIGNED(4) gdt_reg {
		static_cast<uint16_t>(_gdt.size() * sizeof _gdt[0] - 1),
		reinterpret_cast<uint32_t>(&_gdt[0])
	};

	asm volatile (
			"lgdt %0          \n"
			"jmp 1f           \n"
			"1:               \n"
			"movw %%ax,  %%ss \n"
			"movw %%ax,  %%ds \n"
			"movw %%ax,  %%es \n"
			"movw %%ax,  %%fs \n"
			"movw %%ax,  %%gs \n"
			::
			"m"  (gdt_reg),
			"ax" (buildSelector(Privilege::kernel, Segment::kernel_data))
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
