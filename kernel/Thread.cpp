#include <Thread.h>

#include <memory/PagingScope.h>
#include <devices/Serial.h>
#include <devices/CPU.h>
#include <devices/Interrupts.h>

List<Blocker> runnable_threads;
List<Blocker> dying_threads;
Thread kernel_thread;
Thread *current_thread = &kernel_thread;

void Thread::switch_thread(Thread& from, Thread& to){
    asm("pushfl\n"
        "pushl %%ebp\n"
        "movl %%esp, %[esp_from]\n"
        "movl %[esp_to], %%esp\n"
        "movl $switch_resume, %[eip_from]\n"
        "jmp *%[eip_to]\n"
        "switch_resume:\n"
        "popl %%ebp\n"
        "popfl\n"
        : [ esp_from ] "=m"(from.stack_ptr), [ eip_from ] "=m"(from.resume_ptr)
        : [ esp_to ] "m"(to.stack_ptr), [ eip_to ] "r"(to.resume_ptr)
        : "memory");
}

static void asm_dummy(){
	// We need to pop the blocker and then return to
	// the intended starting ip.
	asm("kernel_thread_trampoline:\n"
		"	add %0, %%esp\n"
		"	ret\n"
		:: "i"(sizeof(Blocker))
		);

	asm("user_thread_trampoline:\n"
		"	add %0, %%esp\n"
		"	pop %%ds\n"
		"	pop %%es\n"
		"	pop %%gs\n"
		"	pop %%fs\n"
		"	popa\n"
		"	add $8, %%esp\n"
		"	iret\n"
		:: "i"(sizeof(Blocker))
		);
}
extern "C" void kernel_thread_trampoline(void);
extern "C" void user_thread_trampoline(void);

Thread::Thread() {}

Thread::Thread(uintptr_t stack, uintptr_t start) {
	stack_top = stack;
	stack_ptr = stack;
	push_on_stack<uintptr_t>(start);
	push_on_stack<Blocker>(Blocker(this));

	set_pdir(kernel_page_directory());

	Blocker* blocker = (Blocker*)stack_ptr;
	resume_ptr = (uintptr_t)kernel_thread_trampoline;

	set_state(ThreadState::Runnable);
	runnable_threads.insert(blocker);
}

Thread::Thread(File* executable){
	stack_top = (uintptr_t)kmemalign(0x1000, 0x1000) + 0x1000;
	stack_ptr = stack_top;
	resume_ptr = (uintptr_t)user_thread_trampoline;

	PTE* page_dir = (PTE*)kmemalign(0x1000, 0x1000);
	initialize_page_directory(page_dir);
	set_pdir(page_dir);

	uintptr_t u_stack_top = 0xFF000000;
	uintptr_t u_stack_bottom = u_stack_top - 0xf000;
	uintptr_t exec_start = 0x40000000;
	uintptr_t exec_end = executable->size();
	Region* exec_region = new Region("executable", exec_start, exec_end);
	Region* stack_region = new Region("user_stack", u_stack_bottom, u_stack_top);

	m_user_regions.insert(exec_region);
	m_user_regions.insert(stack_region);
	//TODO: create a heap region

	// Swap into page table and write the executable.
	// TODO: we could possibly have a hypothetical "inode-backed region"
	// instead.
	{
		PagingScope scope(*this);
		executable->read((char*)exec_region->get_start(), 0, executable->size());
	}

	Registers regs;
	//TODO: fill in registers
	regs.cs = 0x1B;
	regs.ss = 0x23;
	regs.ds = 0x23;
	regs.es = 0x23;
	regs.gs = 0x23;
	regs.fs = 0x23;

	regs.ebp = 0; //??
	regs.esp = (uint32_t)u_stack_top;
	regs.eip = exec_start;
	regs.flags |= 0x200; // enable interrupts

	push_on_stack<Registers>(regs);
	push_on_stack<Blocker>(Blocker(this));
	Blocker* blocker = (Blocker*)stack_ptr;
	set_state(ThreadState::Runnable);
	runnable_threads.insert(blocker);
}

Thread* Thread::get_current(){
	return current_thread;
}

void Thread::wait_for_cpu(Blocker& cpu_blocker){
	set_state(ThreadState::Runnable);
	runnable_threads.insert_end(&cpu_blocker);
}

void Thread::wait_on_list(List<Blocker> &list){
	Blocker list_blocker(this);
	set_state(ThreadState::Blocked);
	list.insert_end(&list_blocker);
	yield();
}

void Thread::wake_from_list(List<Blocker> &list){
	if(list.is_empty())
		return;

	Blocker *blocker = list.pop();
	blocker->get_thread()->set_state(ThreadState::Runnable);
	runnable_threads.insert_end(blocker);
}

void Thread::yield(){

	if(runnable_threads.is_empty()){
		com1().write_string("empty yield()\n");
		return;
	}

	com1().write_string("about to switch\n");
	Thread *old_thread = current_thread;
	current_thread = runnable_threads.pop()->get_thread();
	if(current_thread == old_thread)
		return;

	Blocker cpu_blocker(old_thread);
	if(old_thread->get_state() == ThreadState::Running)
		old_thread->wait_for_cpu(cpu_blocker);

	current_thread->set_state(ThreadState::Running);
	current_thread->set_remaining_ticks(current_thread->get_default_ticks());

	//set the kernel tss
	get_tss().esp0 = current_thread->stack_top;
	//swap into the new threads page directory
	uintptr_t new_cr3 = v_to_p((uintptr_t)current_thread->get_pdir());
	set_user_regions(&current_thread->m_user_regions);
	load_cr3(new_cr3);

	switch_thread(*old_thread, *current_thread);
}

void Thread::die(){
	Blocker death_blocker(current_thread);
	current_thread->set_state(ThreadState::Dead);
	dying_threads.insert_end(&death_blocker);
	yield();
}

static void tick_callback(Registers& registers){
	int ticks = current_thread->get_remaining_ticks();
	current_thread->set_remaining_ticks(ticks-1);
}

void Thread::initialize(){
	//We set up the kernel thread to be the current running thread.
	current_thread = &kernel_thread;
	current_thread->set_state(ThreadState::Running);
	current_thread->set_remaining_ticks(current_thread->get_default_ticks());
	current_thread->set_pdir(kernel_page_directory());

	register_interrupt_callback(tick_callback, 0x20);
}
