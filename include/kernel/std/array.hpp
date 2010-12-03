/*
 * array.hpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef ARRAY_H
#define ARRAY_H

#include <kernel/types.h>
#include <kernel/std/algo.hpp>
#include <initializer_list>

#include KERNEL_STD_ARRAY_DEBUG
#include KERNEL_STD_ARRAY_CHECK

namespace kernel {
namespace std {
namespace buffer {

template <typename T, size_t SIZE, uintptr_t ADDR>
struct inplace {
	static_assert(ADDR == 0, "ADDR shouldn't be used!");
	T buffer[SIZE];
};

template <typename T, size_t SIZE, uintptr_t ADDR>
struct absolute {
	static_assert(ADDR != 0, "ADDR shouldn't be null!");
	constexpr static T* buffer = reinterpret_cast<T*>(ADDR);
	constexpr absolute() = default;
	absolute(const absolute&) = delete;
	absolute& operator=(const absolute& from) = delete;
};

template <typename T, size_t SIZE, uintptr_t ADDR>
struct dynamic {
	static_assert(ADDR == 0, "ADDR shouldn't be used!");
	T* buffer;
#ifdef CHECK_ON
	constexpr dynamic(): buffer(nullptr) { }
#else
	constexpr dynamic() = default;
#endif
	constexpr dynamic(uintptr_t addr): buffer(reinterpret_cast<T*>(addr)) { }
	dynamic(const dynamic&) = delete;
	dynamic& operator=(const dynamic& from)
	{
		if (buffer != from.buffer)
			for (size_t i = 0; i < SIZE; ++i)
				buffer[i] = from.buffer[i];
		return *this;
	}
};

} // namespace buffer

inline namespace details {

template <typename T>
struct fill_info { const T& value; };

} // namespace details

template <typename T>
details::fill_info<T> fill(const T& value)
{
	return details::fill_info<T>{value};
}

template <typename T, size_t SIZE,
		 template<typename, size_t, uintptr_t> class BUFFER = buffer::inplace,
		 uintptr_t ADDR = 0>
class array
{
	public:
		typedef       T& reference;
		typedef const T& const_reference;

		array() { dbg("constructor()"); }
		array(uintptr_t addr): _buffer(addr) {
			dbg("constructor(uintptr_t %)", addr);
		}
		array(void* addr): _buffer(reinterpret_cast<uintptr_t>(addr)) {
			dbg("constructor(void* %)", addr);
		}
		array(::std::initializer_list<T> list)
		{
			dbg("constructor(::std::initializer_list<T> %)", list);
			size_t i = 0;
			for (auto item : list)
			{
				if (i >= SIZE)
				{
					dbg("constructor(::std::initializer_list<T> %)"
							" list too big (% > %), truncated!",
							list, list.size(), SIZE);
					break;
				}
				(*this)[i++] = item;
			}
		}
		template <typename IT>
			array(IT begin, IT end)
			{
				dbg("constructor(begin %, end %)", begin, end);
				size_t i = 0;
				while (begin != end)
				{
					if (i >= SIZE)
					{
						dbg("constructor(begin %, end %)"
								" list too big, truncated!", begin, end);
						break;
					}
					(*this)[i++] = *begin++;
				}
			}
		array(const details::fill_info<T>& fill_info)
		{
			dbg("constructor(fill %)", fill_info.value);
			fill(*this, fill_info.value);
		}
		array(uintptr_t addr, const details::fill_info<T>& fill_info): _buffer(addr) {
			dbg("constructor(uintptr_t % fill %)", addr, fill_info.value);
			fill(*this, fill_info.value);
		}
		array(void* addr, const details::fill_info<T>& fill_info):
			_buffer(reinterpret_cast<uintptr_t>(addr)) {
			dbg("constructor(void* % fill %)", addr, fill_info.value);
			fill(*this, fill_info.value);
		}

		reference operator[](size_t idx)
		{
			assert(idx < SIZE);
			assert(_buffer.buffer != 0);
			dbg("operator[](%) -> %", idx, _buffer.buffer[idx]);
			return _buffer.buffer[idx];
		}

		const_reference operator[](size_t idx) const
		{
			assert(idx < SIZE);
			assert(_buffer.buffer != 0);
			dbg("operator[](%) const -> %", idx, _buffer.buffer[idx]);
			return _buffer.buffer[idx];
		}

		constexpr size_t size() const { return SIZE; }

#ifndef CHECK_ON
		typedef       T* iterator;
		typedef const T* const_iterator;

		iterator begin() { return &_buffer.buffer[0]; }
		iterator end() { return &_buffer.buffer[SIZE]; }
		const_iterator begin() const { return &_buffer.buffer[0]; }
		const_iterator end() const { return &_buffer.buffer[SIZE]; }
#else
		template <typename ARRAY, typename REF, typename PTR>
		struct iterator_impl
		{
			ARRAY* _array;
			size_t _idx;

			constexpr iterator_impl(ARRAY* a, size_t idx):
				_array(a), _idx(idx) {}
			iterator_impl(const iterator_impl&) = default;
			iterator_impl& operator=(const iterator_impl&) = default;

		  template <typename IT>
				iterator_impl(const IT& from): _array(from._array), _idx(from._idx) {}
			
			template <typename IT>
				iterator_impl& operator=(const IT& from) {
					_array = from._array; _idx = from._idx; return *this;
				}

			iterator_impl& operator++() {
				++_idx; return *this;
			}
			iterator_impl operator++(int) {
				auto tmp = *this; ++_idx; return tmp;
			}
			iterator_impl& operator+=(int add) {
				_idx += add;
				return *this;
			}
			iterator_impl operator+(int add) const {
				auto ret = *this;
				return ret += add;
			}

			iterator_impl& operator--() {
				--_idx; return *this; }
			iterator_impl operator--(int) {
				auto tmp = *this; --_idx; return tmp;
			}
			iterator_impl& operator-=(int add) {
				_idx -= add;
				return *this;
			}
			iterator_impl operator-(int add) const {
				auto ret = *this;
				return ret -= add;
			}

			bool operator==(const iterator_impl& from) const {
				return _array == from._array and _idx == from._idx; }
			bool operator!=(const iterator_impl& from) const {
				return not (*this == from); }

			bool operator>(const iterator_impl& from) const {
				return _array == from._array and _idx > from._idx; }
			bool operator<(const iterator_impl& from) const {
				return _array == from._array and _idx < from._idx; }

			bool operator>=(const iterator_impl& from) const {
				return _array == from._array and _idx >= from._idx; }
			bool operator<=(const iterator_impl& from) const {
				return _array == from._array and _idx <= from._idx; }

			REF operator*() { return (*_array)[_idx]; }
			REF operator*() const { return (*_array)[_idx]; }

			PTR operator->() { return &(*_array)[_idx]; }
			PTR operator->() const { return &(*_array)[_idx]; }
		};

		typedef iterator_impl<      array,       T&,       T*> iterator;
		typedef iterator_impl<const array, const T&, const T*> const_iterator;

		friend bool operator==(const iterator& a, const const_iterator& b) {
			return a._array == b._array and a._idx == b._idx; }
		friend bool operator!=(const iterator& a, const const_iterator& b) {
			return not (a == b); }
		friend bool operator==(const const_iterator& a, const iterator& b) {
			return a._array == b._array and a._idx == b._idx; }
		friend bool operator!=(const const_iterator& a, const iterator& b) {
			return not (a == b); }

		friend bool operator<(const iterator& a, const const_iterator& b) {
			return a._array == b._array and a._idx < b._idx; }
		friend bool operator>(const iterator& a, const const_iterator& b) {
			return a._array == b._array and a._idx > b._idx; }
		friend bool operator<(const const_iterator& a, const iterator& b) {
			return a._array == b._array and a._idx < b._idx; }
		friend bool operator>(const const_iterator& a, const iterator& b) {
			return a._array == b._array and a._idx > b._idx; }

		friend bool operator<=(const iterator& a, const const_iterator& b) {
			return a._array == b._array and a._idx <= b._idx; }
		friend bool operator>=(const iterator& a, const const_iterator& b) {
			return a._array == b._array and a._idx >= b._idx; }
		friend bool operator<=(const const_iterator& a, const iterator& b) {
			return a._array == b._array and a._idx <= b._idx; }
		friend bool operator>=(const const_iterator& a, const iterator& b) {
			return a._array == b._array and a._idx >= b._idx; }

		iterator begin() { return iterator(this, 0); }
		iterator end() { return iterator(this, SIZE); }
		const_iterator begin() const { return const_iterator(this, 0); }
		const_iterator end() const { return const_iterator(this, SIZE); }
#endif

	private:
		BUFFER<T, SIZE, ADDR> _buffer;
};

} // namespace std
} // namespace kernel

#include <check_off.h>
#include <debug_off.h>

#endif /* ARRAY_H */
