/*
 * segmentation.h
 * Copyright © 2011, François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include <array>
#include <enumcast>

#include KERNEL_SEGMENTATION_DEBUG
#include KERNEL_SEGMENTATION_CHECK

namespace kernel {
namespace segmentation {

enum class Privilege      { BITS = 2, kernel = 0, userland = 3 };
enum class Table          { BITS = 1, global = 0, local = 1 };
enum class SegmentType    { BITS = 4, code = 0b1011, data = 0b0011 };
enum class OperationSize  { BITS = 1, mode16 = 0, mode32 = 1 };

union Descriptor
{
	enum class Type        { BITS = 1, system = 0, datacode = 1 };
	enum class Granularity { BITS = 1, bytes = 0, pages = 1 };

	struct {
		uint16_t limit_15_0;            // Segment limit, bits 15..0
		uint16_t base_paged_addr_15_0;  // Base address, bits 15..0

		uint8_t  base_paged_addr_23_16; // Base address bits 23..16

		uint8_t  segment_type:4;        // Section 3.4.3.1 (code/data)
		//    and 3.5 (system) of Intel x86 vol 3

		uint8_t  descriptor_type:1;     // Descriptor::Type
		uint8_t  privilege:2;           // Privilege
		bool     present:1;             // Present in memory

		uint8_t  limit_19_16:4;         // Segment limit, bits 19..16
		uint8_t  custom:1;              // For custom use.
		uint8_t  zero:1;                // Only for IA-32e, must be 0 however.
		uint8_t  op_size:1;             // OperationSize
		uint8_t  granularity:1;         // Descriptor::Granularity

		uint8_t base_paged_addr_31_24;  // Base address bits 31..24
	};
	struct {
		uint32_t value_high;
		uint32_t value_low;
	};

	Descriptor(): value_high(0), value_low(0) {}

	// build a flat model segment.
	Descriptor(Privilege p, SegmentType st):
		segment_type(std::enum_cast(st)),
		descriptor_type(std::enum_cast(Type::datacode)),
		privilege(std::enum_cast(p)),
		present(true),
		zero(0),
		op_size(std::enum_cast(OperationSize::mode32)),
		granularity(std::enum_cast(Granularity::pages))
	{
		setBase(0);
		setLimit(0xFFFFF);
	}

	inline void setLimit(uint32_t l) {
		limit_15_0  = l;
		limit_19_16 = l >> 16;
	}

	inline void setBase(uint32_t b) {
		base_paged_addr_15_0  = b;
		base_paged_addr_23_16 = b >> 16;
		base_paged_addr_31_24 = b >> 24;
	}
} PACKED ALIGNED(8);

struct Selector
{
	uint8_t  requestedPrivilegeLevel: 2; // Privilege
	uint8_t  tableIndicator: 1;          // Table
	uint16_t index: 12;                  // 4 bytes aligned

	inline Privilege getRequestPrivilegeLevel() const {
		return std::enum_cast<Privilege>(requestedPrivilegeLevel);
	}
	inline Table getTable() const {
		return std::enum_cast<Table>(tableIndicator);
	}
	inline uint16_t getIndex() const { return index; }

	inline void setRequestPrivilegeLevel(Privilege p) {
		requestedPrivilegeLevel = std::enum_cast(p);
	}
	inline void setTable(Table t) {
		tableIndicator = std::enum_cast(t);
	}
	inline void setIndex(uint16_t idx) {
		index = idx;
	}

	Selector() = default;
	constexpr Selector(Privilege requestedPrivilegeLevel,
			Table table, uint16_t index):
		requestedPrivilegeLevel(static_cast<uint8_t>(requestedPrivilegeLevel)),
		tableIndicator(static_cast<uint8_t>(table)),
		index(index)
		{}
} PACKED;

class Manager
{
public:
	enum class Segment {
		null          = 0,
		kernel_code   = 1,
		kernel_data   = 2,
		userland_code = 3,
		userland_data = 4,
		COUNT = 5
	};

	Manager();
	~Manager();

	Manager(const Manager& from) = default;
	Manager(Manager&& from) = default;
	Manager& operator=(const Manager& from) = default;
	Manager& operator=(Manager&& from) = default;

	inline Descriptor& getSegmentDesc(Segment s) {
		return _gdt[static_cast<size_t>(s)];
	}
	
	inline Descriptor& operator[](Segment s) {
		return getSegmentDesc(s);
	}

	static inline constexpr Selector buildSelector(Privilege rpl, Segment s) {
		return Selector(rpl, Table::global, static_cast<uint16_t>(s));
	}
private:
	std::array<Descriptor,
		static_cast<size_t>(Segment::COUNT)
			> _gdt;
};

} // namespace segmentation

extern kernel::segmentation::Manager segmentationManager;

} // namespace kernel

#include <check_off.h>
#include <debug_off.h>

#endif /* SEGMENTATION_H */
