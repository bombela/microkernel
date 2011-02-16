/*
 * memory.h
 * Copyright © 2011, François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef MEMORY_H
#define MEMORY_H

#include <kernel/types.h>
#include <forward>

#include KERNEL_MEM_DEBUG
#include KERNEL_MEM_CHECK

namespace kernel {
namespace memory {

enum class Privilege      { BITS = 2, kernel = 0, userland = 3 };
enum class OperationSize  { BITS = 1, mode16 = 0, mode32 = 1 };

static const uintptr_t page_size = 4096;

struct Page { uint8_t data[page_size]; };

namespace details {

template <typename A, typename B>
struct same_constness { typedef B type; };

template <typename A, typename B>
struct same_constness<const A, B> { typedef const B type; };

template <typename T>
struct def_ptr { typedef typename same_constness<T, uint8_t*>::type type; };

template <typename T>
struct def_ptr<T*> { typedef T* type; };

} // namespace details

template <typename T>
class Addr
{
	public:
		typedef
			typename details::def_ptr<T>::type
			ptr_t;
		typedef
			typename details::same_constness<ptr_t, uintptr_t>::type
			addr_t;

		inline constexpr Addr(T addr):
			_addr(reinterpret_cast<addr_t>(reinterpret_cast<ptr_t>(addr))) {}
		
		inline constexpr Addr(addr_t addr): _addr(addr) {}
		
		inline Addr& operator=(ptr_t addr) {
			_addr = reinterpret_cast<addr_t>(addr);
			return *this;
		}

		inline constexpr operator ptr_t() const {
			return reinterpret_cast<ptr_t>(_addr); }

		inline Addr aligned_up(uintptr_t bnd) const {
			assert((bnd bitand (bnd - 1)) == 0);
			return Addr((_addr + bnd) bitand ~(bnd - 1));
		}
		
		inline Addr aligned_down(unsigned bnd) const {
			assert((bnd bitand (bnd - 1)) == 0);
			return Addr(_addr bitand ~(bnd - 1));
		}
		
		inline Addr aligned_up() const { return aligned_up(page_size); }
		inline Addr aligned_down() const { return aligned_down(page_size); }

		inline size_t distance(const Addr& b) const {
			if (_addr > b._addr)
				return (_addr - b._addr) / sizeof *(ptr_t)0;
			return (b._addr - _addr) / sizeof *(ptr_t)0;
		}

		template <typename U>
			Addr<U> cast() {
				return Addr<U>(_addr);
			}
		
		template <typename U>
			Addr<U> cast() const {
				return Addr<U>(_addr);
			}

	private:
		addr_t _addr;
};

template <typename T>
constexpr inline Addr<T> addr(T addr) { return {addr}; }

namespace details {

template <typename T>
struct def_addr { typedef Addr<T> type; };

template <typename T>
struct def_addr<Addr<T>> { typedef Addr<T> type; };

/*
template <typename T>
struct def_addr {
	typedef
		typename _def_addr<typename std::decay<T>::type, T>::type
		type;
};*/

} // namespace details

template <typename T>
class Range
{
	public:
		typedef typename details::def_addr<T>::type   addr_t;
		typedef typename addr_t::ptr_t                ptr_t;
		
		constexpr Range(addr_t begin, addr_t end):
			_begin(begin),
			_end(end) {}

		inline addr_t& begin() { return _begin; }
		constexpr inline const addr_t& begin() const { return _begin; }
		
		inline addr_t& end() { return _end; }
		inline const addr_t& end() const { return _end; }
		
		inline size_t size() const { return _end.distance(_begin); }
		
		inline Range aligned(unsigned bnd) const {
			return Range(_begin.aligned_down(bnd), _begin.aligned_up(bnd));
		}

		inline Range aligned() const {
			return Range(_begin.aligned_down(), _begin.aligned_up());
		}

		template <typename U>
			Range<U> cast() {
				return Range<U>(
						_begin.cast<U>(),
						_end.cast<U>()
						);
			}

		template <typename U>
			Range<U> cast() const {
				return Range<U>(
						_begin.cast<U>(),
						_end.cast<U>()
						);
			}
		
		inline Range<Page*> pages() {
			return aligned().cast<Page*>();
		}

		inline Range<const Page*> pages() const {
			return aligned().cast<const Page*>();
		}

		friend std::ostream& operator<<(std::ostream& os, const Range& r)
		{
			os("Range(%-%, size=%, sizeof=%, pages=%)",
					r._begin, r._end, r.size(),
					sizeof *(ptr_t)0, r.pages().size());
			return os;
		}
	
	private:
		addr_t _begin;
		addr_t _end;
};

template <typename A, typename B>
constexpr inline Range<A> range(A begin, B end) { return {begin, end}; }

} // namespace memory
} // namespace kernel

#include <check_off.h>
#include <debug_off.h>

#endif /* MEMORY_H */
