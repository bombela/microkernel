/*
 * is_printable.hpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef IS_PRINTABLE_H
#define IS_PRINTABLE_H

// Can we do (object stream of type S) << (object of type T) ?
template <typename T, typename S>
class is_printable
{
	typedef char yes;
	struct no { char _d[2]; };

	template <size_t> struct helper {};

	template <typename U>
	static yes check(U*, helper<sizeof (*(S*)0 << *(U*)0)>* r = 0);

	static no check(...);
	
	public:
		static const bool value = (sizeof check((T*)0) == sizeof (yes));
};

#endif /* IS_PRINTABLE_H */
