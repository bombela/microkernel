/*
 * task.cpp
 * Copyright © 2011, François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <kernel/task.h>
#include <bind>

#include KERNEL_TASK_DEBUG
#include KERNEL_TASK_CHECK

namespace kernel {
namespace task {
	
extern "C" pagination::Page __b_kernel_boot_stack;
extern "C" pagination::Page __e_kernel_boot_stack;

Manager::Manager(): _current(_kthreads.end()) {}

void Manager::init(
		phymem::Manager* phymem,
		pagination::Manager* pmngr,
		interrupt::Manager* ints,
		pic::Manager* pic
		)
{
	_phymem = phymem;
	_mem = pmngr;
	_ints = ints;
	_pic = pic;

	std::fill(_kthreads, Thread());

	dbg("creating initial kernel task...");

	auto ti = newThread();
	assert(ti != _kthreads.end());
	auto& kmain = *ti;

	kmain = Thread(&_mem->kernelContext(),
			stack_t(&__b_kernel_boot_stack, &__e_kernel_boot_stack)
			);

	kmain.setState(State::running);
	_current = ti;

	_ints->setHandler(_pic->irq2int(0),
			std::bind(&Manager::reshedule, this, _1, _2, _3));
	_pic->enable(0);
	dbg("task manager running!");
}

void Manager::reshedule(int idx, int, void**)
{
	auto ti = _current;
	assert(ti != _kthreads.end());

	for (;;)
	{
		if (++ti == _kthreads.end())
			ti = _kthreads.begin();

		if (ti->state() == State::destroying)
			destroyKernelThread(&*ti);

		if (ti->state() == State::running)
			break;
		
		if (ti->state() == State::starting)
			break;
	}

	_pic->eoi(_pic->int2irq(idx));
	if (ti != _current)
		switchTo(ti);
}

extern "C" void task_trampoline(Thread* t, Manager* m) {
	t->setState(State::running);
	t->ep()();
	m->destroyKernelThread(t);
	panic(); // should never reach here
};

void Manager::switchTo(kthreads_t::iterator ti)
{
	dbg("switch from % to %", *_current, *ti);
	assert(ti->state() == State::running
			or ti->state() == State::starting);

	auto previous = _current;
	_current = ti;

	// change memory context
	if (previous->mem() != _current->mem())
		_mem->useContext(_current->mem());
	
	asm volatile (R"(
		/* exchange stack */
		mov %%esp, (%3)
		mov (%4), %%esp

		cmp $0, %0
		je 1f
		
		/* starting, bootstrap on the new stack */
		push %1 /* push &manager */
		push %2 /* push &thread */
		pushl $0 /* fake eip for task_trampoline */
		
		pushf /* EFLAGS with cli */
		popl %1
		orl $0x200, %1 /* sti */
		pushl %1 /* EFLAGS with sti */

		push %%cs /* cs */
		push $task_trampoline /* EIP */
		iret

		1:
		/* running, just continue on the new stack */
			)"
			::
				"a" (ti->state() == State::starting),
				"r" (this),
				"r" (&*ti),
				"r" (&previous->sp()),
				"r" (&_current->sp())
			);
}
 
Thread* Manager::createKernelThread(const entrypoint_t& ep)
{
	auto ti = newThread();
	assert(ti != _kthreads.end());
	auto& kthread = *ti;

	auto stackpage = pagination::vaddr(_phymem->alloc())
		.cast<pagination::Page*>();
	assert(stackpage != nullptr);
	auto stack = stack_t(stackpage, stackpage + 1);

	if (not _mem->kernelContext().present(stackpage))
		_mem->kernelContext().map(stackpage);
	
	kthread = Thread(&_mem->kernelContext(), stack, ep);
	return &kthread;
}

void Manager::destroyKernelThread(Thread* t)
{
	auto ti = _kthreads.begin() + (t - &_kthreads[0]);
	assert(ti >= _kthreads.begin() and ti < _kthreads.end());
	
	dbg("destroying %", *t);
	if (_current == ti)
	{
		ti->setState(State::destroying);
		yield();
		panic(); // should never reach here
	}

	for (auto& p : ti->getStack())
	{
		ti->mem().unmap(&p);
		_phymem->free(&p);
	}
	ti->setState(State::noninit);
}

void Manager::yield()
{
	// TODO implement clean switch
	
	struct {
		uint8_t _int;
		uint8_t idx;
		uint8_t _ret;
	} PACKED volatile intcode { 0xcd, _pic->irq2int(0) , 0xc3 };
	((void (*)()) &intcode)();
}

} // namespace task
} // namespace kernel
