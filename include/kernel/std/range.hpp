/*
 * range.hpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef RANGE_H
#define RANGE_H

namespace kernel {
namespace std {

template <typename C>
auto begin(C&& container) -> decltype(container.begin())
{
	return container.begin();
}

template <typename C>
auto end(C&& container) -> decltype(container.end())
{
	return container.end();
}

} // namespace std
} // namespace kernel

#endif /* RANGE_H */
