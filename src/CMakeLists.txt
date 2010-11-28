#
# src/CMakeLists.txt
# Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
#
#

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=gnu++0x -m32 -fPIC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostdlib -nostdinc")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fno-exceptions")
	
	set(CMAKE_ASM-ATT_FLAGS "${CMAKE_ASM-ATT_FLAGS} --32")
endif()

set(SRCS
	kernel.lds
	multiboot.s
	main.cpp
	)

add_executable(kern0x ${SRCS})
target_link_libraries(kern0x gcc)
set_target_properties(kern0x PROPERTIES
	LINK_FLAGS "-Wl,--script=${CMAKE_CURRENT_SOURCE_DIR}/kernel.lds")

add_custom_target(cpykern
	"${TOOLS_DIRECTORY}/cpykern.sh"
	"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/kern0x" "${TEST_IMG}"
	DEPENDS kern0x
	)

add_custom_target(run kvm -hda "${TEST_IMG}")
add_dependencies(run cpykern)

add_custom_target(rungdb
	"${TOOLS_DIRECTORY}/gdb.sh"
	"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/kern0x" "${TEST_IMG}"
	DEPENDS kern0x
	)

add_custom_target(gdb
	"${TOOLS_DIRECTORY}/gdb.sh"
	"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/kern0x"
	DEPENDS kern0x
	)
