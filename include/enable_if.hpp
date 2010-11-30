/*
 * enable_if.hpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef ENABLE_IF_H
#define ENABLE_IF_H

template <bool COND, typename T = void*>
struct enable_if { typedef T type; };

template <typename T>
struct enable_if<false, T> { };

#endif /* ENABLE_IF_H */
