/*
 * pagination.h
 * Copyright © 2011, François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef PAGINATION_H
#define PAGINATION_H

#include <kernel/phymem.h>

#include KERNEL_PAGINATION_DEBUG
#include KERNEL_PAGINATION_CHECK

namespace kernel {
namespace pagination {

struct Page: memory::Page {};

// page table
struct TableEntry
{
	bool present:        1;
	bool write:          1;
	bool useraccess:     1;
	bool write_trough:   1;
	bool cache_disabled: 1;
	bool accessed:       1;
	bool dirty:          1;
	bool paging_typing:  1; // support not enabled
	bool global:         1; // support not enabled
	uint32_t ignored:    3;
	uint32_t page_addr: 20;

	inline TableEntry(void* p_addr = 0):
		present(false),
		write(true),
		useraccess(false),
		write_trough(false),
		cache_disabled(true),
		accessed(false),
		dirty(false),
		paging_typing(false),
		global(false),
		page_addr(reinterpret_cast<uint32_t>(p_addr) >> 12)
		{}

	inline void setPage(const void* p_addr) {
		page_addr = reinterpret_cast<uint32_t>(p_addr) >> 12;
	}
	
	inline Page& getPage() const {
		return *reinterpret_cast<Page*>(page_addr << 12);
	}
} PACKED;

typedef std::array<TableEntry,
		memory::page_size / sizeof (TableEntry)> Table;

// page directory
struct DirectoryEntry
{
	bool present:         1;
	bool write:           1;
	bool useraccess:      1;
	bool write_trough:    1;
	bool cache_disabled:  1;
	bool accessed:        1;
	bool ignored1:        1;
	bool zero:            1;
	uint32_t ignored:     4;
	uint32_t table_addr: 20;

	inline DirectoryEntry(void* t_addr = 0):
		present(false),
		write(true),
		useraccess(false),
		write_trough(false),
		cache_disabled(true),
		accessed(false),
		zero(0),
		table_addr(reinterpret_cast<uint32_t>(t_addr) >> 12)
		{}
	
	inline void setTable(void* t_addr) {
		table_addr = reinterpret_cast<uint32_t>(t_addr) >> 12;
	}

	inline Table& getTable() const {
		return *reinterpret_cast<Table*>(table_addr << 12);
	}

	inline TableEntry& operator[](size_t idx) const {
		return getTable()[idx];
	}
} PACKED;

union LinearAddr {
	struct {
		uint32_t offset    :12;
		uint32_t table     :10;
		uint32_t directory :10;
	};
	uint32_t addr;
	LinearAddr(void* addr): addr(reinterpret_cast<uint32_t>(addr)) {}
	template <typename T>
		LinearAddr(memory::Addr<T*> addr):
			addr(reinterpret_cast<uint32_t>(static_cast<const void*>(addr))) {}

	friend std::ostream& operator<<(std::ostream& os, const LinearAddr& la) {
		return os("%x(d=%, t=%, o=%)",
				la.addr, la.directory, la.table, la.offset);
	}
};

template <typename T>
class VirtualAddr: public memory::Addr<T>
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

		inline constexpr VirtualAddr(): super_t() {}
		inline constexpr VirtualAddr(T addr): super_t(addr) {}
		inline constexpr VirtualAddr(memory::Octet addr): super_t(addr) {}
		inline constexpr VirtualAddr(addr_t addr): super_t(addr) {}

		inline operator LinearAddr() const { return LinearAddr(*this); }
		inline uint32_t offset() const { return LinearAddr(*this).offset; }
		inline uint32_t table() const { return LinearAddr(*this).table; }
		inline uint32_t directory() const { return LinearAddr(*this).directory; }

		page_ref& page() const {
			return *reinterpret_cast<page_ptr>(
					ptr_t(this->aligned_down())
					);
		}
};

template <typename T>
constexpr inline VirtualAddr<T> vaddr(T addr) { return {addr}; }

class Manager
{
	public:
		Manager(phymem::Manager* phymem): _phymem(phymem) {}
		void init();

		const pagination::Page* map(const pagination::Page* vp)
		{
			identity_map(vp);
			return vp;
		}
		
		pagination::Page* map(const pagination::Page* vp,
				const phymem::Page* pp)
		{
			auto& page = resolveTableEntry(vp);
			assert(not page.present);
			page.setPage(pp);
			page.present = true;
		}
		
		void unmap(const pagination::Page* vp)
		{
			//TODO
		}

	private:
		phymem::Manager* _phymem;
		
		std::array<DirectoryEntry,
				memory::page_size / sizeof (DirectoryEntry),
				std::buffer::dynamic> _directory;

		Page* palloc() {
			Page* page = reinterpret_cast<Page*>(_phymem->alloc());
			assert(page != nullptr);
			return page;
		}

		void identity_map(const Page*);
		TableEntry& resolveTableEntry(const Page* p);
};

} // namespace pagination
} // namespace kernel

#include <check_off.h>
#include <debug_off.h>

#endif /* PAGINATION_H */
