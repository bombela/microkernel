/*
 * static_if.hpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef STATIC_IF_H
#define STATIC_IF_H

template <bool COND, typename THEN, typename ELSE>
struct static_if { typedef THEN type; };

template <typename THEN, typename ELSE>
struct static_if<false, THEN, ELSE> { typedef ELSE type; };

#endif /* STATIC_IF_H */
