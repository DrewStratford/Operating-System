#include <Thread.h>

#include <devices/Serial.h>

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
}
extern "C" void kernel_thread_trampoline(void);

Thread::Thread() {}

Thread::Thread(uintptr_t stack, uintptr_t start) {
	stack_ptr = stack;
	push_on_stack<uintptr_t>(start);
	push_on_stack<Blocker>(Blocker(this));

	Blocker* blocker = (Blocker*)stack_ptr;
	resume_ptr = (uintptr_t)kernel_thread_trampoline;

	set_state(ThreadState::Runnable);
	runnable_threads.insert(blocker);
}


void Thread::wait_for_cpu(){
	Blocker cpu_blocker(this);
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

	if(old_thread->get_state() == ThreadState::Running)
		old_thread->wait_for_cpu();

	current_thread->set_state(ThreadState::Running);
	switch_thread(*old_thread, *current_thread);
}

void Thread::die(){
	Blocker death_blocker(current_thread);
	current_thread->set_state(ThreadState::Dead);
	dying_threads.insert_end(&death_blocker);
	yield();
}

void Thread::initialize(){
	//We set up the kernel thread to be the current running thread.
	current_thread = &kernel_thread;
	current_thread->set_state(ThreadState::Running);
}
