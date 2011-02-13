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

namespace segmentation {

enum class Privilege      { BITS = 2, kernel = 0, userland = 3 };
enum class SegmentType    { BITS = 4 };
enum class Table          { BITS = 1, global = 0, local = 1 };

struct Descriptor
{
	enum class Type { BITS = 1, system = 0, program = 1 };

	uint16_t   limit_15_0;            // Segment limit, bits 15..0 
	uint16_t   base_paged_addr_15_0;  // Base address, bits 15..0 
	
	uint8_t    base_paged_addr_23_16; // Base address bits 23..16 

	uint8_t segment_type:4;        // Section 3.4.3.1 (code/data)
								   //    and 3.5 (system) of Intel x86 vol 3 

	uint8_t descriptor_type:1;     // 0=system, 1=Code/Data 
	uint8_t privilege:2;                 // Descriptor privilege level 
	uint8_t present:1;

	uint8_t limit_19_16:4;         // Segment limit, bits 19..16 
	uint8_t custom:1;
	uint8_t zero:1;
	uint8_t op_size:1;             // 0=16bits instructions, 1=32bits 
	uint8_t granularity:1;         // 0=limit in bytes, 1=limit in pages 

	uint8_t base_paged_addr_31_24; // Base address bits 31..24 
} PACKED ALIGNED(8);

union Selector
{
	struct {
		uint16_t index: 12;
		uint8_t  tableIndicator: 1;
		uint8_t  requestPrivilegeLevel: 2;
	};
	uint16_t value;

	inline uint16_t getIndex() const { return index << 3; }
	inline Table getTable() const {
		return std::enum_cast<Table>(tableIndicator);
	}
	inline Privilege getRequestPrivilegeLevel() const {
		return std::enum_cast<Privilege>(requestPrivilegeLevel);
	}

	inline void setIndex(uint16_t idx) {
		index = idx >> 3;
	}
	inline void setTable(Table t) {
		tableIndicator = std::enum_cast(t);
	}
	inline void setRequestPrivilegeLevel(Privilege p) {
		requestPrivilegeLevel = std::enum_cast(p);
	}

	Selector() = default;
	Selector(uint16_t index,
			Table table, Privilege requestPrivilegeLevel):
		index(index >> 3), tableIndicator(std::enum_cast(table)),
		requestPrivilegeLevel(std::enum_cast(requestPrivilegeLevel))
		{}
} PACKED;

class Manager
{
public:
	Manager();
	~Manager();

	Manager(const Manager& from) = default;
	Manager(Manager&& from) = default;
	Manager& operator=(const Manager& from) = default;
	Manager& operator=(Manager&& from) = default;

	/*enum class Privilege { kernel = 0, userland = 3 };
	enum class DescTable { global, local };
	enum class Segment   {
		KERNEL_CODE, KERNEL_DATA, KERNEL_TSS,
		USER_CODE, USER_DATA
	};*/
	
	//inline uint16_t buildSegmentSelector(Privilege pl,
	//		DescTable dt, Segment seg) {
	//			
	//		}
private:
	//std::array<SegmentDescriptor, 16> _;
};

} // namespace segmentation

#endif /* SEGMENTATION_H */
