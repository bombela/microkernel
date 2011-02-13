/*
 * segmentation.h
 * Copyright © 2011, François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include <array>

class SegmentationManager
{
public:
	SegmentationManager();
	~SegmentationManager();

	SegmentationManager(const SegmentationManager& from) = default;
	SegmentationManager(SegmentationManager&& from) = default;
	SegmentationManager& operator=(const SegmentationManager& from) = default;
	SegmentationManager& operator=(SegmentationManager&& from) = default;

	enum class Privilege { kernel = 0, userland = 3 };
	enum class DescTable { global, local };
	enum class Segment   {
		KERNEL_CODE, KERNEL_DATA, KERNEL_TSS,
		USER_CODE, USER_DATA
	};
	
	inline uint16_t buildSegmentSelector(Privilege pl,
			DescTable dt, Segment seg) {
				
			}

private:
	std::array<> _;
};

#endif /* SEGMENTATION_H */
