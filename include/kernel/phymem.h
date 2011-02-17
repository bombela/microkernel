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

		void assign(const BoolArray& o) { _buffer.assign(o); }
	private:
		buffer_t _buffer;

		inline size_t cntbit(type v) const {
			v = v - ((v >> 1) & (type)~(type)0/3);
			v = (v & (type)~(type)0/15*3) + ((v >> 2) & (type)~(type)0/15*3);
			v = (v + (v >> 4)) & (type)~(type)0/255*15;
			return (type)(v * ((type)~(type)0/255)) >> (sizeof(type) - 1) * 8;
		}
};

#ifndef CHECK_ON
	#define assertCanBeFree(x)
#endif

class Manager
{
	public:
		void init(unsigned mem_lower_kb, unsigned mem_upper_kb);

		inline void free(memory::Page* p) {
			dbg("page=%", p->number());
			assertCanBeFree(p);
			_free(reinterpret_cast<Page*>(p));
		}

		void free(const memory::PageRange& range) {
			assert(range.is_aligned());
			dbg("range=%", range);
			_free(range.cast<Page*>());
		}
		
		void printMemUsage() const;

		// return 0 if failed.
		memory::Page* alloc() {
			Page* page = _alloc();
			dbg("allocated page=%", page->page.number());
			return &page->page;
		}
		
		memory::PageRange alloc(size_t cnt) {
			if (_free_begin == 0)
			{
				dbg("out of memory");
				return memory::PageRange();
			}
			// TODO
			return memory::PageRange();
		}

		void testAllocator();
	private:
		union Page
		{
			memory::Page page;
			struct
			{
				Page* prev;
				Page* next;
			};

			inline void hook(Page* prev, Page* next)
			{
				this->prev = prev;
				this->next = next;
				if (prev)
					prev->next = this;
				if (next)
					next->prev = this;
			}
		};

		std::array<Page, 0x1,
			std::buffer::absolute_resizable, 0x0>_pages;
		BoolArray<0x1,
			std::buffer::dynamic_resizable>_map;
		Page* _free_begin;

#ifdef CHECK_ON
		memory::PageRange _kmem;
		memory::PageRange _vmem;
		memory::PageRange _biosmem;

		void assertCanBeFree(memory::Page* p) const {
			assert(p != nullptr);
			assert(not _kmem.contain(p));
			assert(not _biosmem.contain(p));
			assert(not _map.range().pages().contain(p));
		}
#endif // CHECK_ON

		inline void _use(const Page* p) {
			_map.set(p->page.number());
			if (p->prev != 0)
				p->prev->next = p->next;
			if (p->next != 0)
				p->next->prev = p->prev;
		}

		inline Page* _find_prev_free(Page* p) {
			for (size_t i = p->page.number() - 1; i > 0; --i)
				if (not _map.isset(i))
					return &_pages[i];
			return nullptr;
		}
		
		inline Page* _find_next_free(Page* p) {
			for (size_t i = p->page.number() + 1; i < _pages.size(); ++i)
				if (not _map.isset(i))
					return &_pages[i];
			return nullptr;
		}

		inline void _free(Page* p) {
			Page* prev = _find_prev_free(p);
			Page* next = _find_next_free(p);

			p->hook(prev, next);

			if (_free_begin == next)
				_free_begin = p;
			_map.clr(p->page.number());
		}

		void _free(const memory::Range<Page*>& range) {
			for (auto& p : range) {
				_free(&p);
			}
		}

		inline bool used(const Page& p) const {
			return _map.isset(p.page.number());
		}

		void rebuildFreeList();
		size_t cntFreePage() const;

		Page* _alloc() {
			if (_free_begin == 0)
			{
				dbg("out of memory");
				return 0;
			}
			Page* page = _free_begin;
			_free_begin = _free_begin->next;
			_use(page);
			return page;
		}
};

} // namespace phymem
} // namespace kernel

#ifdef CHECK_ON
#undef assertCanBeFree
#endif

#include <check_off.h>
#include <debug_off.h>

#endif /* PHYMEM_H */
