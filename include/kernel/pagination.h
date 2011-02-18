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

// cr3
struct Cr3Reg
{
	bool ignored1:            3;
	bool write_trough:        1;
	bool cache_disabled:      1;
	bool ignored2:            7;
	uint32_t directory_addr: 20;

	Cr3Reg(void* pd_addr):
		write_trough(false),
		cache_disabled(true),
		directory_addr(reinterpret_cast<uint32_t>(pd_addr) >> 12)
	{}
};

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
	bool ignored:        3;
	uint32_t page_addr: 20;

	TableEntry(void* p_addr):
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
};

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
	bool ignored:         4;
	uint32_t table_addr: 20;

	DirectoryEntry(void* t_addr = 0):
		present(false),
		write(true),
		useraccess(false),
		write_trough(false),
		cache_disabled(true),
		accessed(false),
		zero(0),
		table_addr(reinterpret_cast<uint32_t>(t_addr) >> 12)
		{}
	
	void setTable(void* t_addr) {
		table_addr = reinterpret_cast<uint32_t>(t_addr) >> 12;
	}
};

struct Page
{
	uint8_t data[memory::page_size];
};

union LinearAddr {
	struct {
		uint32_t offset    :20;
		uint32_t table     :10;
		uint32_t directory :10;
	};
	uint32_t addr;
	LinearAddr(void* addr): addr(reinterpret_cast<uint32_t>(addr)) {}
	template <typename T>
		LinearAddr(memory::Addr<T*> addr):
			addr(reinterpret_cast<uint32_t>(
						typename memory::Addr<T*>::ptr_t(addr)
						)) {}
};

template <typename T>
class VirtualAddr: public memory::Addr<T>
{
	public:
		typedef memory::Addr<T> super_t;
		typedef typename super_t::addr_t addr_t;

		inline constexpr VirtualAddr(): super_t() {}
		inline constexpr VirtualAddr(T addr): super_t(addr) {}
		inline constexpr VirtualAddr(memory::Octet addr): super_t(addr) {}
		inline constexpr VirtualAddr(addr_t addr): super_t(addr) {}

		inline operator LinearAddr() const { return LinearAddr(*this); }
		inline uint32_t offset() const { return LinearAddr(*this).offset; }
		inline uint32_t table() const { return LinearAddr(*this).table; }
		inline uint32_t directory() const { return LinearAddr(*this).directory; }
};

template <typename T>
constexpr inline VirtualAddr<T> vaddr(T addr) { return {addr}; }

class Manager
{
	public:
		Manager(phymem::Manager* phymem): _phymem(phymem) {}
		void init();

	private:
		phymem::Manager* _phymem;
		
		std::array<DirectoryEntry,
				memory::page_size / sizeof (DirectoryEntry),
				std::buffer::dynamic> _directory;

		void* palloc() {
			void* page = _phymem->alloc();
			assert(page != nullptr);
			return page;
		}

		Page* identity_map(memory::Page*);
};

} // namespace pagination
} // namespace kernel

#include <check_off.h>
#include <debug_off.h>

#endif /* PAGINATION_H */
