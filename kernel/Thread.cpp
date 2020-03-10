#include <Thread.h>

#include <devices/Serial.h>

List<Thread> runnable_threads;
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
	runnable_threads.insert(this);
}

Thread::Thread(uintptr_t stack, uintptr_t start) {
	stack_ptr = stack;
	resume_ptr = start;
	runnable_threads.insert(this);
}

void Thread::yield(){

	if(runnable_threads.is_empty()){
		com1().write_string("empty yield()\n");
		return;
	}

	com1().write_string("about to switch\n");
	Thread *old_thread = current_thread;
	current_thread = runnable_threads.pop();
	runnable_threads.insert_end(old_thread);
	switch_thread(*old_thread, *current_thread);
}

void Thread::initialize(){
	//We set up the kernel thread to be the current running thread.
	runnable_threads.remove(&kernel_thread);
	current_thread = &kernel_thread;
}
