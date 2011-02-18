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
#include <function>

#include KERNEL_PHYMEM_DEBUG
#include KERNEL_PHYMEM_CHECK

namespace kernel {
namespace phymem {

struct Page: memory::Page {};

template <typename T>
class Physicaladdr: public memory::Addr<T>
{
	public:
		typedef memory::Addr<T> super_t;
		typedef typename super_t::addr_t addr_t;
		typedef typename super_t::ptr_t  ptr_t;
		typedef
			typename memory::Addr<T>::template same_constness<Page>::type*
			page_ptr;
		typedef
			typename memory::Addr<T>::template same_constness<Page>::type&
			page_ref;

		inline constexpr Physicaladdr(): super_t() {}
		inline constexpr Physicaladdr(T addr): super_t(addr) {}
		inline constexpr Physicaladdr(memory::Octet addr): super_t(addr) {}
		inline constexpr Physicaladdr(addr_t addr): super_t(addr) {}

		page_ref& page() const {
			return *reinterpret_cast<page_ptr>(
					ptr_t(this->aligned_down())
					);
		}
};

template <typename T>
constexpr inline Physicaladdr<T> paddr(T addr) { return {addr}; }

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
		static const size_t full  = type(-1);

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

		size_t cntset() const {
			size_t cnt = 0;
			for (auto i : _buffer)
				cnt += cntbit(i);
			return cnt;
		}

		// return >= size() if error
		inline size_t find_clr_backward(size_t idx) const
		{
			if (idx == 0)
				return size();
			// iter over bits blocks
			do
			{
				--idx;
				auto& b = _buffer[idx >> shift];
				if (b != full)
				{
					// found a partially used block, slow search.
					size_t min = (idx bitand ~mask);
					for (; idx >= min; --idx)
						if (0 == (b bitand (1 << (idx & mask))))
							return idx;
				}
				idx = (idx bitand ~mask);
			}
			while (idx > 0);
			return size();
		}

		// return >= size() if error
		inline size_t find_clr_forward(size_t idx) const
		{
			// iter over bits blocks
			for (++idx; idx < size(); idx = (idx bitor mask) + 1)
			{
				auto& b = _buffer[idx >> shift];
				if (b != full)
				{
					// found a partially used block, slow search.
					size_t max = (idx bitor mask);
					for (; idx <= max; ++idx)
						if (0 == (b bitand (1 << (idx & mask))))
							return idx;
				}
			} 
			return size();
		}

		void clear() {
			std::fill(_buffer, 0);
		}

		void assign(const BoolArray& o) { _buffer.assign(o); }
	private:
		buffer_t _buffer;

		inline size_t cntbit(type v) const {
			size_t cnt;
			asm ("popcnt %1, %0" : "=r" (cnt) : "g" (v));
			return cnt;
			/*
			v = v - ((v >> 1) & (type)~(type)0/3);
			v = (v & (type)~(type)0/15*3) + ((v >> 2) & (type)~(type)0/15*3);
			v = (v + (v >> 4)) & (type)~(type)0/255*15;
			return (type)(v * ((type)~(type)0/255)) >> (sizeof(type) - 1) * 8;
			*/
		}
};

#ifndef CHECK_ON
	#define assertCanBeFree(x)
#endif

class Manager
{
	public:
		void init(unsigned mem_lower_kb, unsigned mem_upper_kb);
		
		inline void use(const phymem::Page* p) {
			assert(not _map.isset(p->number()));
			_use(reinterpret_cast<const Page*>(p));
		}

		inline bool used(const phymem::Page* p) const {
			return used(*reinterpret_cast<const Page*>(p));
		}
		
		inline void free(void* p) {
			free(reinterpret_cast<phymem::Page*>(p));
		}

		inline void free(phymem::Page* p) {
			dbg("page=%", p->number());
			assertCanBeFree(p);
			_free(reinterpret_cast<Page*>(p));
		}

		void free(const memory::PageRange& range) {
			assert(range.is_aligned());
			dbg("range=%", range);
			_free(range.cast<Page*>());
		}
		
		// return 0 if failed.
		phymem::Page* alloc() {
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

		void printMemUsage() const;
		void testAllocator();

		memory::PageRange mem() const {
			return memory::range(
				&_pages[0], &_pages[0] + _pages.size()
			).pages();
		}
	private:
		union Page
		{
			phymem::Page page;
			struct
			{
				Page* prev;
				Page* next;
			};


			inline void hook(Page* p, Page* n)
			{
				this->prev = p;
				this->next = n;
				if (p)
					p->next = this;
				if (n)
					n->prev = this;
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

		inline void assertCanBeFree(phymem::Page* p) const {
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
			size_t idx = _map.find_clr_backward(p->page.number());
			if (idx >= _map.size())
				return nullptr;
			return &_pages[idx];
		}
		
		inline Page* _find_next_free(Page* p) {
			size_t idx = _map.find_clr_forward(p->page.number());
			if (idx >= _map.size())
				return nullptr;
			return &_pages[idx];
		}

		inline void _free(Page* p) {
			assert(_map.isset(p->page.number()));
			
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
