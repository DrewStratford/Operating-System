#include <Thread.h>

#include <devices/Serial.h>

List<Thread> runnable_threads;
List<Thread> dying_threads;
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

Thread::Thread(){
	reschedule();
}

Thread::Thread(uintptr_t stack, uintptr_t start) {
	stack_ptr = stack;
	resume_ptr = start;
	reschedule();
}

void Thread::reschedule(){
	set_state(ThreadState::Runnable);
	runnable_threads.insert_end(this);
}

void Thread::yield(){

	if(runnable_threads.is_empty()){
		com1().write_string("empty yield()\n");
		return;
	}

	com1().write_string("about to switch\n");
	Thread *old_thread = current_thread;
	current_thread = runnable_threads.pop();
	if(current_thread == old_thread)
		return;

	if(old_thread->get_state() == ThreadState::Running)
		old_thread->reschedule();

	current_thread->set_state(ThreadState::Running);
	switch_thread(*old_thread, *current_thread);
}

void Thread::die(){
	current_thread->set_state(ThreadState::Dead);
	dying_threads.insert_end(current_thread);
	yield();
}

void Thread::initialize(){
	//We set up the kernel thread to be the current running thread.
	runnable_threads.remove(&kernel_thread);
	current_thread = &kernel_thread;
	current_thread->set_state(ThreadState::Running);
}
