/*
 * task.h
 * Copyright © 2011, François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef TASK_H
#define TASK_H

#include <kernel/pagination.h>
#include <kernel/interrupt.h>
#include <kernel/pic.h>
#include <array>
#include <algorithm>

#include KERNEL_TASK_DEBUG
#include KERNEL_TASK_CHECK

namespace kernel {
namespace task {

enum class State { noninit = 0, init,
	 starting, running, stopped, destroying, };

inline std::ostream& operator<<(std::ostream& os, State s) {
	static const char* const tab[] = {
		"noninit", "init",
		"starting", "running", "stopped", "destroying"
	};
	return os << tab[static_cast<int>(s)];
}

typedef std::function<void (), 512> entrypoint_t;
typedef memory::Range<pagination::Page*> stack_t;

class Manager;

class Thread
{
	public:
		Thread():
			_state(State::noninit)
		{}
		
		Thread(
				pagination::Context* mem,
				stack_t stack,
				const entrypoint_t& ep = entrypoint_t()
				):
			_state(State::init),
			_mem(mem),
			_stack(stack),
			_ep(ep),
			_sp(stack.end().cast<void*>())
		{
			dbg("new %", *this);
		}

		State state() const { return _state; }
		void setState(State s) {
			dbg("% now %", *this, s);
			_state = s;
		}

		pagination::Context& mem() const { return *_mem; }

		void*& sp() { return _sp; }
		entrypoint_t& ep() { return _ep; }

		const stack_t& getStack() const {
			return _stack;
		}

		friend std::ostream& operator<<(std::ostream& os, const Thread& t) {
			return os("thread(%) status=% stack=%", &t, t._state, t._stack);
		}

		Thread* start() {
			assert(_state == State::init or _state == State::stopped);
			if (_state == State::init)
				_state = State::starting;
			else if (_state == State::stopped)
				_state = State::running;
			return this;
		}
		Thread* stop() {
			assert(_state == State::running);
			if (_state == State::running)
				_state = State::stopped;
			return this;
		}

	private:
		State                                      _state;
		pagination::Context*                       _mem;
		stack_t                                    _stack;
		entrypoint_t                               _ep;
		void*                                      _sp;
};

class Manager
{
	public:
		Manager();
		void init(
				phymem::Manager* phymem,
				pagination::Manager* pmngr,
				interrupt::Manager* ints,
				pic::Manager* pic
				);

		Thread* createKernelThread(const entrypoint_t&,
				pagination::Context* pc = 0);
		
		template <typename F>
			Thread* createKernelThread(const F& f,
				pagination::Context* pc = 0) {
				return createKernelThread(entrypoint_t(f), pc);
			}

		void destroyKernelThread(Thread* t);
		void yield();
		Thread* current() const { return &*_current; }

		size_t runningThreads() const {
			size_t cnt = 0;
			for (auto& t: _kthreads)
				if (t.state() == State::running)
					++cnt;
			return cnt;
		}
	private:
		typedef std::array<Thread, 256> kthreads_t;

		kthreads_t              _kthreads;
		kthreads_t::iterator    _current;
		phymem::Manager*        _phymem;
		pagination::Manager*    _mem;
		interrupt::Manager*     _ints;
		pic::Manager*           _pic;

		kthreads_t::iterator newThread() {
			return std::find_if(_kthreads, [](Thread& t) {
					return t.state() == State::noninit;
					});
		}

		void reshedule(int, int, void**);
		void switchTo(kthreads_t::iterator ti);
};

} // namespace task

extern task::Manager taskManager;

} // namespace kernel

#include <check_off.h>
#include <debug_off.h>

#endif /* TASK_H */
