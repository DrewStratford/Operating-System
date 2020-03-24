#pragma once

#include <stdint.h>

#include <data/List.h>

enum ThreadState
	{ Running
	, Runnable
	, Blocked
	, Dead
	};

class Thread : public ListNode<Thread>{
public:
	Thread();
	Thread(uintptr_t stack, uintptr_t resume);
	static void switch_thread(Thread& from, Thread& to);
	static void yield();
	static void die();
	static void initialize();

	void reschedule();
	ThreadState get_state() const { return state; };
	void set_state(ThreadState new_state) { state = new_state; };

private:
	uintptr_t stack_ptr { 0 };
	uintptr_t resume_ptr { 0 };
	ThreadState state;
};

