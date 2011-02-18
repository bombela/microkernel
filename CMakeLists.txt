#
# CMakeLists.txt
# Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
#
#

cmake_minimum_required(VERSION 2.8)

###############################################################################
# COMPILER FLAGS
###############################################################################

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wswitch-enum")

	set(USE_LTO CACHE BOOL "Enable Link Time Optimization")
	if (USE_LTO)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
	endif()
endif()

###############################################################################
# CONFIGURATION
###############################################################################

enable_language(ASM-ATT)

set(TOOLS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tools")

set(TEST_IMG "${CMAKE_CURRENT_BINARY_DIR}/test.img"
	CACHE FILEPATH "hdd image used for testing")

set(KERNEL_ALL_DEBUG CACHE BOOL "All debug messages")
set(KERNEL_ALL_CHECK CACHE BOOL "All runtime assertions")

set(KERNEL_DEFINES)
macro(kern0x_add_define defname shortdoc)
	set(def_debug "KERNEL_${defname}_DEBUG")
	set(def_check "KERNEL_${defname}_CHECK")

	set(${def_debug} CACHE BOOL "${shortdoc} debug messages")
	if (KERNEL_ALL_DEBUG OR ${def_debug})
		message(STATUS "${shortdoc} debug messages     ON")
		list(APPEND KERNEL_DEFINES "${def_debug}=\"debug_on.h\"")
	else()
		list(APPEND KERNEL_DEFINES "${def_debug}=\"debug_off.h\"")
	endif()
	
	set(${def_check} CACHE BOOL "${shortdoc} runtime assertions")
	if (KERNEL_ALL_CHECK OR ${def_check})
		message(STATUS "${shortdoc} runtime assertions ON")
		list(APPEND KERNEL_DEFINES "${def_check}=\"check_on.h\"")
	else()
		list(APPEND KERNEL_DEFINES "${def_check}=\"check_off.h\"")
	endif()
endmacro()

kern0x_add_define(STD_ARRAY "std::array")
kern0x_add_define(STD_STACK "std::stack")
kern0x_add_define(STD_LIST  "std::list")
kern0x_add_define(STD_OSTREAM  "std::ostream")
kern0x_add_define(STD_IOSTREAM  "std::cout & co")
kern0x_add_define(STD_FUNCTION  "function holder")
kern0x_add_define(STD_ENUMCAST  "enum_cast feature")
kern0x_add_define(CXXRUNTIME  "C++ runtime support")
kern0x_add_define(CONSOLE  "standard console")
kern0x_add_define(SEGMENTATION  "segmentation manager")
kern0x_add_define(INTERRUPT  "interrupt manager")
kern0x_add_define(PIC  "PIC manager")
kern0x_add_define(MEM  "memory tools")
kern0x_add_define(PHYMEM  "Physical memory manager")
kern0x_add_define(PAGINATION  "Pagination manager")

set(MEM_FN_VERSION "static" CACHE STRING
	"mem_fn implemetation version (storepmf or static)")
message(STATUS "mem_fn implementation: ${MEM_FN_VERSION}")
string(TOUPPER "${MEM_FN_VERSION}" MEM_FN_VERSION)
list(APPEND KERNEL_DEFINES "PREFER_MEM_FN_${MEM_FN_VERSION}")

###############################################################################
# SUBDIRS
###############################################################################

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)
