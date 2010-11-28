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
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

###############################################################################
# CONFIGURATION
###############################################################################

enable_language(ASM-ATT)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(TOOLS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tools")

set(TEST_IMG "${CMAKE_CURRENT_BINARY_DIR}/test.img"
	CACHE FILEPATH "hdd image used for testing")

include_directories("include")

###############################################################################
# SUBDIRS
###############################################################################

add_subdirectory(src)
