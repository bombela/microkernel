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
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=gnu++0x -Wno-pmf-conversions")
endif()

###############################################################################
# UTILITIES_SRCS
###############################################################################

set(UTILITIES_SRCS
	${CMAKE_SOURCE_DIR}/src/basename.cpp
	)

###############################################################################
# BOOST CONFIGURATION
###############################################################################

set(Boost_USE_STATIC_LIBS   OFF)
set(Boost_USE_MULTITHREADED ON)
set(BOOST_INCLUDEDIR "/usr/local/include")
set(Boost_ADDITIONAL_VERSIONS 1.45 1.44 1.43 1.42 1.41 1.40 1.39 1.38)
add_definitions(-DBOOST_ALL_NO_LIB)


find_package(Boost 1.38 COMPONENTS system unit_test_framework test_exec_monitor)

if (Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
	link_libraries(${Boost_LIBRARIES})

	macro(kern0x_add_test src)
		get_filename_component(name ${src} NAME_WE)
		set(name "test_${name}")

		set_property(SOURCE ${src} APPEND
			PROPERTY COMPILE_DEFINITIONS ${KERNEL_DEFINES} "USE_EXTERNAL_STL=1"
			)

		add_executable(${name} ${src} ${ARGN} ${UTILITIES_SRCS})
		add_test(${name} ${name})
	endmacro()
else()
	message(AUTHOR_WARNING "Boost testing framework not found, no kern0x::std tests will be built")
	macro(kern0x_add_test src)
	endmacro()
endif()

###############################################################################
# CONFIGURATION
###############################################################################

# override some kernel headers by purpose
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

# kernel include
include_directories("${CMAKE_SOURCE_DIR}/include")

###############################################################################
# RULES
###############################################################################

foreach(src
		array.cpp
		is_printable.cpp
		stack.cpp
		range.cpp
		algo.cpp
		bind.cpp
		function.cpp
		)
	kern0x_add_test(${src})
endforeach()

kern0x_add_test(memfn.cpp memfn_noinline_test.cpp)
