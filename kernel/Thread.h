#pragma once

#include <stdint.h>

#include <devices/CPU.h>
#include <filesystem/FileSystem.h>
#include <memory/Paging.h>
#include <memory/Region.h>
#include <data/List.h>
#include <Signal.h>

enum ThreadState
	{ Running
	, Runnable
	, Blocked
	, Dead
	, Reaped
	};

class Blocker;
class WaitBlocker;

enum SignalDisposition{
	Ignore,
	Die,
	Callback
};

class SignalHandler{
public:
	SignalHandler(){};
	SignalHandler(uintptr_t callback){
		this->disposition = Callback;
		this->callback = callback;
	}

	uintptr_t get_callback(){
		return callback;
	}
	void set_disposition(SignalDisposition disposition){
		this->disposition = disposition;
	}
	SignalDisposition get_disposition(){
		return disposition;
	}

private:
	SignalDisposition disposition { Die };
	uintptr_t callback { 0 };
	uintptr_t alt_stack { 0 };
};

class Signal : public ListNode<Signal> {
public:
	int signal_id = 0;
	Signal(int signal) : signal_id(signal) {
	}
};

class Thread : public ListNode<Thread>{
public:
	Thread();
	Thread(uintptr_t stack, uintptr_t resume);
	Thread(File&, size_t inode_count, int* inodes, const string&);

	~Thread();

	void setup_arguments(const string& arguments);

	template <typename T>
	void push_on_stack(T t){
		stack_ptr -= sizeof(T);
		*(T*)stack_ptr = t;
	};

	template <typename T>
	void push_on_user_stack(T t){
		auto& regs = get_registers();
		regs.esp -= sizeof(T);
		*(T*)regs.esp = t;
	};

	static Thread* lookup(int tid);
	static void switch_thread(Thread& from, Thread& to);
	static void yield();
	void die();
	static void initialize();
	static Thread* get_current();

	Registers& get_registers();
	bool should_die() { return m_should_die; };
	int exit_status() { return m_exit_status; };
	void mark_for_death(int ret) {
		m_should_die = true;
		m_exit_status = ret;
	}

	template<typename blocker>
	auto wait_on_list(List<blocker>& list, Lock& lock){
		blocker list_blocker(this);
		set_state(ThreadState::Blocked);
		lock.lock();
		list.insert_end(&list_blocker);
		lock.unlock();
		yield();
		return list_blocker.finish();
	}

	template<typename blocker>
	static void wake_from_list(List<blocker>& list, Lock& lock){
		lock.lock();
		if(list.is_empty()){
			lock.unlock();
			return;
		}

		blocker* block = list.pop();
		lock.unlock();
		block->get_thread()->set_state(ThreadState::Runnable);
		get_runnable_threads().insert_end((Blocker*)block);
	};

	ThreadState get_state() const { return state; };
	void set_state(ThreadState new_state) { state = new_state; };

	void set_default_ticks(int ticks) { default_ticks = ticks; };
	int get_default_ticks() { return default_ticks; };
	void set_remaining_ticks(int ticks) { remaining_ticks = ticks; };
	int get_remaining_ticks() { return remaining_ticks; };

	PTE* get_pdir() { return pdir; };
	void set_pdir(PTE* new_pdir) { pdir = new_pdir; };

	uint32_t get_tid() { return tid; };
	uint32_t get_parent_tid() { return parent->get_tid(); };
	Thread* get_parent() { return parent; };

	string& get_name() { return m_name; };
	void set_name(const string& str){ m_name = str;};
	string& get_current_directory() { return m_current_directory; };
	void set_current_directory(const string& str){ m_current_directory = str;};

	List<Region> m_user_regions;
	List<WaitBlocker> waiters;

	Inode* get_inode(int32_t);
	int32_t insert_inode(Inode*);
	int32_t open_file(char*);
	void close_file(int32_t);

	void set_handler(int signal, SignalHandler handler);
	SignalDisposition signal(int signal);
	static SignalDisposition send_signal(int tid, int signal);
	void handle_signals();

private:
	static List<Blocker>& get_runnable_threads();
	static List<Blocker>& get_dying_threads();
	uintptr_t stack_bottom { 0 };
	uintptr_t stack_top { 0 };
	uintptr_t stack_ptr { 0 };
	uintptr_t resume_ptr { 0 };
	ThreadState state;

	string m_name;
	string m_current_directory;

	uint32_t tid { 0 };
	Thread* parent { nullptr };

	bool m_should_die { false };
	int m_exit_status { 0 };

	PTE* pdir = nullptr;

	int default_ticks { 4 };
	int remaining_ticks { 0 };

	void wait_for_cpu(Blocker&);

	bool is_ancestor(Thread& thread);

	#define MAX_INODES 20
	Inode* m_inodes[MAX_INODES] = { nullptr };

	SignalHandler sig_handlers[SIGMAX];
	List<Signal> pending_signals;

	static uint32_t tid_allocator;
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
	void finish(){ return; };
protected:
	Thread *thread;
	BlockerStatus status { Waiting };
};

class WaitBlocker : public Blocker{
public:
	WaitBlocker(Thread* t) : Blocker(t) {};
	void set_return(int ret){ ret_val = ret; };
	int finish(){ return ret_val; };
private:
	int ret_val { 0 };
};

extern Thread *current_thread;
