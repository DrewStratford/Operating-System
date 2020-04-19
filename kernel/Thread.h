#pragma once

#include <stdint.h>

#include <filesystem/FileSystem.h>
#include <memory/Paging.h>
#include <memory/Region.h>
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
	Thread(File*);

	template <typename T>
	void push_on_stack(T t){
		stack_ptr -= sizeof(T);
		*(T*)stack_ptr = t;
	};

	static void switch_thread(Thread& from, Thread& to);
	static void yield();
	static void die();
	static void initialize();
	static Thread* get_current();

	void wait_on_list(List<Blocker>&);
	static void wake_from_list(List<Blocker>&);
	ThreadState get_state() const { return state; };
	void set_state(ThreadState new_state) { state = new_state; };

	void set_default_ticks(int ticks) { default_ticks = ticks; };
	int get_default_ticks() { return default_ticks; };
	void set_remaining_ticks(int ticks) { remaining_ticks = ticks; };
	int get_remaining_ticks() { return remaining_ticks; };

	PTE* get_pdir() { return pdir; };
	void set_pdir(PTE* new_pdir) { pdir = new_pdir; };

	List<Region> m_user_regions;

private:
	uintptr_t stack_top { 0 };
	uintptr_t stack_ptr { 0 };
	uintptr_t resume_ptr { 0 };
	ThreadState state;

	PTE* pdir = nullptr;

	int default_ticks { 5 };
	int remaining_ticks { 0 };

	void wait_for_cpu(Blocker&);
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
