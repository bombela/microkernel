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

enum class Privilege {
	kernel_rw,
	kernel_ro,
	user_rw,
	user_ro,
};

enum class Present {
   	yes, no	
};

struct Page: memory::Page {} PACKED;

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
	
	inline phymem::Page* getPage() const {
		return reinterpret_cast<phymem::Page*>(table_addr << 12);
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
		inline constexpr VirtualAddr(ptr_t addr): super_t(addr) {}
		inline constexpr VirtualAddr(memory::Octet addr): super_t(addr) {}
		inline constexpr VirtualAddr(addr_t addr): super_t(addr) {}

		inline operator LinearAddr() const { return LinearAddr(*this); }
		inline uint32_t offset() const { return LinearAddr(*this).offset; }
		inline uint32_t table() const { return LinearAddr(*this).table; }
		inline uint32_t directory() const { return LinearAddr(*this).directory; }

		page_ptr page() const {
			return reinterpret_cast<page_ptr>(
					ptr_t(this->aligned_down())
					);
		}
};

template <typename T>
constexpr inline VirtualAddr<T> vaddr(T addr) { return {addr}; }

class Context
{
	public:
		Context() {}

		Context(phymem::Manager* phymem): _phymem(phymem) {
			_directory.relocate(palloc());
			std::fill(_directory, DirectoryEntry());
			dbg("directory=%", &_directory[0]);
			std::fill(_tablesCounter, 0);
			map(pagination::vaddr(getDirectoryAddr()).page());
		}

		~Context() {
			if (_directory.addr())
			{
				for (auto& table: _directory)
					if (table.present)
						_phymem->free(table.getPage());
				_phymem->free(&_directory[0]);
			}
		}

		Context(const Context& from) = delete;
		Context& operator=(const Context& from) = delete;
		
		Context(Context&& from):
			_phymem(from._phymem),
			_directory(std::move(from._directory)),
			_tablesCounter(std::move(from._tablesCounter))
		{}
		
		Context& operator=(Context&& from)
		{
			_phymem = from._phymem;
			_directory = std::move(from._directory);
			_tablesCounter = std::move(from._tablesCounter);
			return *this;
		}

		const pagination::Page* map(const pagination::Page* vp,
				Privilege p = Privilege::kernel_rw,
				Present m = Present::yes)
		{
			return map(vp, phymem::paddr(vp).cast<phymem::Page*>(), p, m);
		}
		
		const pagination::Page* map(const pagination::Page* vp,
				const phymem::Page* pp,
				Privilege p = Privilege::kernel_rw,
				Present m = Present::yes)
		{
			auto& page = increfTable(vp);
			assert(not page.present);
			remap(vp, pp, p, m);
			return vp;
		}

		const pagination::Page* remap(const pagination::Page* vp,
				Privilege p = Privilege::kernel_rw,
				Present m = Present::yes)
		{
			return remap(vp, 0, p, m);
		}

		const pagination::Page* remap(const pagination::Page* vp,
				const phymem::Page* pp,
				Privilege p = Privilege::kernel_rw,
				Present m = Present::yes)
		{
			auto va = vaddr(vp);
			assert(va.is_aligned());

			auto& table = _directory[va.directory()];
			assert(table.present);
			
			auto& page = table[va.table()];

			if (pp)
				page.setPage(pp);
			page.present = (m == Present::yes);
			page.useraccess = (p == Privilege::user_rw
					or p == Privilege::user_ro);
			page.write = (p == Privilege::kernel_rw
					or p == Privilege::user_rw);
			return vp;
		}

		void unmap(const pagination::Page* vp)
		{
			auto va = vaddr(vp);
			assert(va.is_aligned());

			auto& table = _directory[va.directory()];
			assert(table.present);

			auto& page = table[va.table()];
			assert(page.present);
			page.present = false;

			decrefTable(vp);
		}

		DirectoryEntry* getDirectoryAddr() {
			return &_directory[0];
		}

	private:
		phymem::Manager* _phymem;
		
		std::array<DirectoryEntry,
				memory::page_size / sizeof (DirectoryEntry),
				std::buffer::dynamic, 0> _directory;

		std::array<int16_t, memory::page_size / sizeof (TableEntry)
			> _tablesCounter;
		
		TableEntry& increfTable(const pagination::Page* p)
		{
			auto va = vaddr(p);
			assert(va.is_aligned());

			auto& table = _directory[va.directory()];
			if (not table.present)
			{
				// allocate a table page
				Table* tablePage = reinterpret_cast<Table*>(palloc());
				assert(tablePage);
				table.setTable(tablePage);
				table.present = true;
				std::fill(*tablePage, TableEntry());
			}
			++_tablesCounter[va.directory()];
			return table[va.table()];
		}

		void decrefTable(const Page* p)
		{
			auto va = vaddr(p);
			assert(va.is_aligned());

			auto& table = _directory[va.directory()];
			assert(table.present == true);

			assert(_tablesCounter[va.directory()] > 0);
			if (--_tablesCounter[va.directory()] == 0)
			{
				table.present = 0;
				_phymem->free(table.getPage());
			}
		}

		phymem::Page* palloc()
		{
			phymem::Page* page = _phymem->alloc();
			if (page == 0)
			{
				std::cout("kernel out of memory");
				panic();
			}
			return page;
		}
};

extern "C" phymem::Page __b_kernel;
extern "C" phymem::Page __e_kernel;

class Manager
{
	public:
		~Manager();
		void init(phymem::Manager* phymem);

		Context& kernelContext() {
			return _kernelContext;
		}

		Context& currentContext() {
			return *_currentContext;
		}

		void useContext(Context& c);

		void useKernelContext() {
			dbg("switch to kernel context");
			useContext(kernelContext());
		}

		Context newContext();
	private:
		phymem::Manager* _phymem;
		Context          _kernelContext;
		Context*         _currentContext;
};

} // namespace pagination
} // namespace kernel

#include <check_off.h>
#include <debug_off.h>

#endif /* PAGINATION_H */
