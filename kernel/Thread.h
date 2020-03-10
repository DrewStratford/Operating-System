#pragma once

#include <stdint.h>

#include <data/List.h>

class Thread : public ListNode<Thread>{
public:
	Thread();
	Thread(uintptr_t stack, uintptr_t resume);
	static void switch_thread(Thread& from, Thread& to);
	static void yield();
	static void initialize();

private:
	uintptr_t stack_ptr { 0 };
	uintptr_t resume_ptr { 0 };
};

