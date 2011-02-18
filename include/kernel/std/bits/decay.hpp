/*
 * decay.hpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef DECAY_H
#define DECAY_H

namespace std {
namespace kernel_std {

template <typename T>
struct remove_ptr {
   	typedef T type;	
};

template <typename T>
struct remove_ptr<T*> {
   	typedef T type;
};

template <typename T>
struct remove_all_ptr {
   	typedef T type;	
};

template <typename T>
struct remove_all_ptr<T*> {
   	typedef typename remove_all_ptr<T>::type type;
};

template <typename T>
struct remove_ref {
   	typedef T type;	
};

template <typename T>
struct remove_ref<T&> {
   	typedef T type;
};

template <typename T>
struct remove_ref<T&&> {
   	typedef T type;
};

template <typename T>
struct remove_const {
   	typedef T type;	
};

template <typename T>
struct remove_const<const T> {
   	typedef T type;
};

template <typename T>
struct decay {
	typedef
		typename remove_all_ptr<
			typename remove_const<
				typename remove_ref<T
					>::type
				>::type
			>::type
		type;
};


} // namespace kernel_std
} // namespace std

#endif /* DECAY_H */
