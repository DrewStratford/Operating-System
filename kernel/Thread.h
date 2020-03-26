#pragma once

#include <stdint.h>

#include <data/List.h>

enum ThreadState
	{ Running
	, Runnable
	, Blocked
	, Dead
	};

class Blocker;

class Thread : public ListNode<Thread>{
public:
	Thread();
	Thread(uintptr_t stack, uintptr_t resume);

	template <typename T>
	void push_on_stack(T t){
		stack_ptr -= sizeof(T);
		*(T*)stack_ptr = t;
	};

	static void switch_thread(Thread& from, Thread& to);
	static void yield();
	static void die();
	static void initialize();

	void wait_for_cpu();
	void wait_on_list(List<Blocker>&);
	static void wake_from_list(List<Blocker>&);
	ThreadState get_state() const { return state; };
	void set_state(ThreadState new_state) { state = new_state; };


private:
	uintptr_t stack_ptr { 0 };
	uintptr_t resume_ptr { 0 };
	ThreadState state;
};

enum BlockerStatus
	{ Ok
	, Waiting
	, Failure
	, Signalled
	};

class Blocker : public ListNode<Blocker>{
public:
	Blocker(Thread *thread){ this->thread = thread; }
	void set_status(BlockerStatus new_status) { status = new_status; };
	BlockerStatus get_status() { return status; };
	Thread* get_thread(){ return thread; };
protected:
	Thread *thread;
	BlockerStatus status { Waiting };
};

extern Thread *current_thread;
