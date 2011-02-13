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

enum class Privilege { kernel = 0, userland = 3, BITS = 2 };

struct Descriptor
{
	uint16_t   limit_15_0;            // Segment limit, bits 15..0 
	uint16_t   base_paged_addr_15_0;  // Base address, bits 15..0 
	
	uint8_t    base_paged_addr_23_16; // Base address bits 23..16 

	enum class SegType {  };
	uint8_t segment_type:4;        // Section 3.4.3.1 (code/data)
								   //    and 3.5 (system) of Intel x86 vol 3 

	enum class DescType { system = 0, program = 1 };
	uint8_t descriptor_type:1;     // 0=system, 1=Code/Data 
	uint8_t dpl:2;                 // Descriptor privilege level 
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
		uint8_t  requestPriviliegeLevel: 2;
	};
	uint16_t value;

	inline void setRequestPrivilegeLevel(Privilege p) {
		tableIndicator = std::enum_cast(p);
	}
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
