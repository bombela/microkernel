/*
 * phymem.h
 * Copyright © 2011, François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef PHYMEM_H
#define PHYMEM_H

#include <kernel/types.h>
#include <kernel/memory.h>
#include <array>

#include KERNEL_PHYMEM_DEBUG
#include KERNEL_PHYMEM_CHECK

namespace kernel {
namespace phymem {

namespace details {

template <typename T, T V, T BASE, bool ZERO = (V / BASE == 0)>
struct log;

template <typename T, T V, T BASE>
struct log<T, V, BASE, false>{
	static const T value = 1 + log<T, V / BASE, BASE, (V / BASE) == 0>::value;
};

template <typename T, T V, T BASE>
struct log<T, V, BASE, true>{
	static const T value = -1;
};

} // namespace details

template <size_t SIZE,
		 template<typename, size_t, uintptr_t>
			 class BUFFER = std::buffer::inplace,
		 uintptr_t ADDR = 0>
class BoolArray
{
	public:
		typedef uint32_t type;
		typedef std::array<type, SIZE, BUFFER, ADDR> buffer_t;
		
	private:
		static const size_t bits  = sizeof (type) * 8;
		static const size_t mask  = bits - 1;
		static const size_t shift = details::log<type, bits, 2>::value;

	public:

		BoolArray() {}
		size_t size() const { return _buffer.size() * bits; }

		inline void set(size_t idx) {
			auto& b = _buffer[idx >> shift];
			b |= (1 << (idx & mask));
		}
		
		inline void clr(size_t idx) {
			auto& b = _buffer[idx >> shift];
			b &= ~(1 << (idx & mask));
		}

		inline bool isset(size_t idx) const {
			auto b = _buffer[idx >> shift];
			return b bitand (1 << (idx & mask));
		}

		memory::Range<const type*> range() const {
			return memory::range(&_buffer[0],
				&_buffer[0] + _buffer.size());
		}

		void relocate(void* addr) {
			_buffer.relocate(addr);			
		}

		void resize(size_t size) {
			_buffer.resize((size + (bits - 1)) / bits);
		}

		size_t cntbit2(size_t v) const
		{
			size_t cnt = 0;
			for (int i = 0; i < 32; ++i)
			{
				if (v & (1 << i))
					++cnt;
			}
			return cnt;
		}

		size_t cntset() const {
			size_t cnt = 0;
			for (auto i : _buffer)
				cnt += cntbit(i);
			return cnt;
		}

		void clear() {
			std::fill(_buffer, 0);
		}
	private:
		buffer_t _buffer;

		inline size_t cntbit(type v) const {
			v = v - ((v >> 1) & (type)~(type)0/3);
			v = (v & (type)~(type)0/15*3) + ((v >> 2) & (type)~(type)0/15*3);
			v = (v + (v >> 4)) & (type)~(type)0/255*15;
			return (type)(v * ((type)~(type)0/255)) >> (sizeof(type) - 1) * 8;
		}
};

class Manager
{
	public:
		void init(unsigned mem_lower_kb, unsigned mem_upper_kb);

		inline void free(const memory::Page& p) {
			dbg("page=%", p.number());
			_free(p);
		}

		void free(const memory::PageRange& range) {
			assert(range.is_aligned());
			dbg("range=%", range);
			for (auto& p : range) {
				_free(p);
			}
		}
		
		inline bool used(const memory::Page& p) {
			return _map.isset(p.number());
		}

		void printMemUsage() const;
	private:
		union Page
		{
			memory::Page page;
			struct
			{
				Page* prev;
				Page* next;
			};
		};

		std::array<Page, 0x1,
			std::buffer::absolute_resizable, 0x0>_pages;
		BoolArray<0x1,
			std::buffer::dynamic_resizable>_map;
		Page* _free_begin;

		inline void use(const memory::Page& p) {
			dbg("page=%", p.number());
			_use(p);
		}

		void use(const memory::PageRange& range) {
			assert(range.is_aligned());
			dbg("range=%", range);
			for (auto& p : range) {
				_use(p);
			}
		}

		inline void _free(const memory::Page& p) {
			_map.clr(p.number());
		}

		inline void _use(const memory::Page& p) {
			_map.set(p.number());
		}

		inline bool used(const Page& p) const {
			return _map.isset(p.page.number());
		}

		void rebuildFreeList();
		size_t cntFreePage() const;
};

} // namespace phymem
} // namespace kernel

#include <check_off.h>
#include <debug_off.h>

#endif /* PHYMEM_H */
