/* vim: ft=cpp
 * config.hpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef CONFIG_H
#define CONFIG_H

namespace std {
#ifdef USE_EXTERNAL_STL
	namespace kernel_std { }
#else
	inline namespace kernel_std { }
#endif
} // namespace std

#endif /* CONFIG_H */
