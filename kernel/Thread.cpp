#include <Thread.h>

#include <string.h>

#include <memory/PagingScope.h>
#include <devices/Serial.h>
#include <devices/Interrupts.h>

List<Thread> all_threads;
List<Blocker> runnable_threads;
List<Blocker> dying_threads;
Thread kernel_thread;
Thread* idle_thread = nullptr;
Thread* current_thread = &kernel_thread;

uint32_t Thread:: tid_allocator = 1;

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
	NoInterrupts i;
	tid = tid_allocator++;
	parent = current_thread;

	stack_top = stack;
	stack_ptr = stack;
	push_on_stack<uintptr_t>(start);
	push_on_stack<Blocker>(Blocker(this));

	set_pdir(kernel_page_directory());

	Blocker* blocker = (Blocker*)stack_ptr;
	resume_ptr = (uintptr_t)kernel_thread_trampoline;

	set_state(ThreadState::Runnable);
	runnable_threads.insert(blocker);

	all_threads.insert(this);
}

Thread::Thread(File& executable, size_t inode_count, int* inodes){
	NoInterrupts d;

	tid = tid_allocator++;
	parent = current_thread;

	stack_top = (uintptr_t)kmemalign(0x1000, 0x1000) + 0xFFF;
	stack_ptr = stack_top;
	resume_ptr = (uintptr_t)user_thread_trampoline;

	PTE* page_dir = (PTE*)kmemalign(0x1000, 0x1000);
	initialize_page_directory(page_dir);
	set_pdir(page_dir);

	// Copy inodes from the current process if neccesary.
	for(size_t i = 0; inodes && i < 20 && i < inode_count; i++){
		insert_inode(current_thread->get_inode(inodes[i]));
	}

	uintptr_t u_stack_size = 0xf000;
	uintptr_t u_stack_top = 0xFF000000;
	uintptr_t u_stack_bottom = u_stack_top - u_stack_size;
	uintptr_t exec_start = 0x40000000;
	uintptr_t exec_size = executable.size();
	UserRegion* exec_region = new UserRegion("executable", exec_start, exec_size);
	UserRegion* stack_region = new UserRegion("user_stack", u_stack_bottom, u_stack_size);

	m_user_regions.insert(exec_region);
	m_user_regions.insert(stack_region);
	//TODO: create a heap region

	// Swap into page table and write the executable.
	// TODO: we could possibly have a hypothetical "inode-backed region"
	// instead.
	{
		PagingScope scope(*this);
		executable.read((char*)exec_region->get_start(), 0, executable.size());
	}

	stack_ptr -= sizeof(Registers);
	Registers* regs = (Registers*)stack_ptr;

	//TODO: fill in registers
	regs->cs = 0x1B;
	regs->ss = 0x23;
	regs->ds = 0x23;
	regs->es = 0x23;
	regs->gs = 0x23;
	regs->fs = 0x23;

	regs->ebp = 0; //??
	regs->esp = (uint32_t)u_stack_top - 16;
	regs->eip = exec_start;
	regs->flags = 0x200; // enable interrupts

	push_on_stack<Blocker>(Blocker(this));
	Blocker* blocker = (Blocker*)stack_ptr;
	set_state(ThreadState::Runnable);
	runnable_threads.insert(blocker);

	all_threads.insert(this);
}

Registers& Thread::get_registers(){
	Registers* rs = (Registers*)stack_top;
	rs--;
	return *rs;
}

Thread* Thread::get_current(){
	return current_thread;
}

List<Blocker>& Thread::get_runnable_threads(){
	return runnable_threads;
}

List<Blocker>& Thread::get_dying_threads(){
	return dying_threads;
}

void Thread::wait_for_cpu(Blocker& cpu_blocker){
	set_state(ThreadState::Runnable);
	runnable_threads.insert_end(&cpu_blocker);
}

/*
void Thread::wait_on_list(List<Blocker> &list){
	Blocker list_blocker(this);
	set_state(ThreadState::Blocked);
	list.insert_end(&list_blocker);
	yield();
}
*/

/*
void Thread::wake_from_list(List<Blocker> &list){
	if(list.is_empty())
		return;

	Blocker *blocker = list.pop();
	blocker->get_thread()->set_state(ThreadState::Runnable);
	runnable_threads.insert_end(blocker);
}
*/

void Thread::yield(){

	Thread *old_thread = current_thread;

	// Create a blocker so that we can store this thread
	// in the runnable threads.
	Blocker cpu_blocker(old_thread);

	// Note that if we don't have any threads to run we just
	// resume the kernel thread.
	current_thread = runnable_threads.is_empty()
		? &kernel_thread
		: runnable_threads.pop()->get_thread();

	if(current_thread == old_thread)
		return;

	// If appropriate, store the thread on the runnable threads queue.
	// Otherwise we assume that another function has properly stored
	// the thread on a queue.
	if(old_thread != &kernel_thread && old_thread->get_state() == ThreadState::Running)
		old_thread->wait_for_cpu(cpu_blocker);

	current_thread->set_state(ThreadState::Running);
	current_thread->set_remaining_ticks(current_thread->get_default_ticks());

	// Set the kernel tss
	get_tss().esp0 = current_thread->stack_top;
	// Swap into the new threads page directory
	uintptr_t new_cr3 = v_to_p((uintptr_t)current_thread->get_pdir());
	set_user_regions(&current_thread->m_user_regions);
	load_cr3(new_cr3);

	switch_thread(*old_thread, *current_thread);
}

SignalDisposition Thread::signal(int signal){
	if(signal >= SIGMAX)
		return Ignore;

	auto disposition = sig_handlers[signal].get_disposition();
	switch(disposition){
		case Die:
			mark_for_death(-1);
			break;
		case Callback:
			pending_signals.insert_end(new Signal(signal));
			break;
		default:
			break;
	}
	return disposition;
}

static void sig_asm_dummy(){
	// assumes a pusha followed by return address on
	// the stack.
	asm volatile("signal_trampoline:\n"
		"popa\n"
		"ret\n"
		"signal_trampoline_end:\n");
}
extern "C" void signal_trampoline();
extern "C" void signal_trampoline_end();

void Thread::handle_signals(){
	if(pending_signals.is_empty())
		return;
	
	Signal* signal = pending_signals.pop();
	SignalHandler handler = sig_handlers[signal->signal_id];

	auto& regs = get_registers();
	auto old_eip = regs.eip;
	auto old_esp = regs.esp;
	size_t trampoline_size =
		(size_t)signal_trampoline_end - (size_t)signal_trampoline;

	// load trampoline onto the user stack
	memcpy((void*)old_esp, (void*)signal_trampoline, trampoline_size);
	regs.esp-= trampoline_size;

	// return from the trampoline
	push_on_user_stack<uint32_t>(old_eip);

	// pusha
	push_on_user_stack<uint32_t>(regs.eax);
	push_on_user_stack<uint32_t>(regs.ecx);
	push_on_user_stack<uint32_t>(regs.edx);
	push_on_user_stack<uint32_t>(regs.ebx);
	push_on_user_stack<uint32_t>(old_esp);
	push_on_user_stack<uint32_t>(regs.ebp);
	push_on_user_stack<uint32_t>(regs.esi);
	push_on_user_stack<uint32_t>(regs.edi);

	// 'call' the handler
	regs.eip = handler.get_callback();

	// return from the handler into the
	// trampoline that's been loaded to the stack
	push_on_user_stack<uint32_t>(old_esp);

	delete signal;
}

void Thread::set_handler(int signal, SignalHandler handler){
	if(signal >= SIGMAX)
		return;
	sig_handlers[signal] = handler;
}

SignalDisposition Thread::send_signal(int tid, int signal){
	if(Thread* thread = Thread::lookup(tid))
		return thread->signal(signal);

	return Ignore;
}

void Thread::die(){
	Blocker death_blocker(this);
	current_thread->set_state(ThreadState::Dead);
	dying_threads.insert_end(&death_blocker);

	// Remove the thread from the all threads list
	this->remove();

	//wake any waiting threads
	while(!waiters.is_empty()){
		auto blocker = waiters.peek();
		blocker->set_return(current_thread->exit_status());
		wake_from_list<WaitBlocker>(waiters);
	}

	yield();
}

Thread* Thread::lookup(int tid){

	for(auto* t = all_threads.peek(); t != nullptr; t = t->next){
		if(t->get_tid() == tid)
			return t;
	}
	return nullptr;
}

bool Thread::is_ancestor(Thread& child){
	Thread* parent = child.get_parent();
	while(parent != nullptr){
		if(parent == this)
			return true;
		parent = parent->get_parent();
	}
	return false;
}

Inode* Thread::get_inode(int32_t fd){
	if(fd < 0 || fd >= MAX_INODES)
		return nullptr;
	return m_inodes[fd];
}

int32_t Thread::insert_inode(Inode* inode){
	if(!inode)
		return -1;

	for(int32_t i = 0; i < MAX_INODES; i++){
		if(m_inodes[i] == nullptr){
			m_inodes[i] = inode;
			return i;
		}
	}
	return -1;
}

int32_t Thread::open_file(char* filepath){
	File* file = root_directory().lookup_file(filepath);
	if(!file)
		return -1;

	return insert_inode(file);
}

void Thread::close_file(int32_t i){
	if(i < MAX_INODES && i >= 0)
		m_inodes[i] = nullptr;
}

static void tick_callback(Registers& registers){
	int ticks = current_thread->get_remaining_ticks();
	current_thread->set_remaining_ticks(ticks-1);
}

static int32_t syscall_create_thread(Registers& registers){
	void** stack = (void**)registers.esp;
	char* filepath = (char*)stack[0];
	size_t inode_count = (size_t)stack[1];
	int* inodes = (int*)stack[2];

	if(File* f = root_directory().lookup_file(filepath)){
		Thread *t = new Thread(*f, inode_count, inodes);
		return t->get_tid();
	}
	return -1;
}

static int32_t syscall_exit_thread(Registers& registers){
	void** stack = (void**)registers.esp;
	int ret_val = (int)stack[0];
	current_thread->mark_for_death(ret_val);
	return -1;
}

static int32_t syscall_signal(Registers& registers){
	char** stack = (char**)registers.esp;
	int32_t signal = (int32_t)stack[0];
	uintptr_t callback = (uintptr_t)stack[1];
	current_thread->set_handler(signal, SignalHandler(callback));
	return 0;
}

static int32_t syscall_kill(Registers& registers){
	char** stack = (char**)registers.esp;
	int32_t signal = (int32_t)stack[0];
	int32_t tid = (int32_t)stack[1];
	
	tid = tid == -1 ? current_thread->get_tid() : tid;
	com1() << "Sending signal(" << signal << ") to: " << tid << "\n";
	Thread::send_signal(tid, signal);

	return 0;
}

int32_t syscall_open_file(Registers& registers){
	char** stack = (char**)registers.esp;
	char* filepath = stack[0];
	int32_t fd = current_thread->open_file(filepath);
	return fd;
}

int32_t syscall_close_file(Registers& registers){
	char** stack = (char**)registers.esp;
	int32_t fd = (int32_t)stack[0];
	current_thread->close_file(fd);

	return 0;
}

int32_t syscall_read_fd(Registers& registers){
	char** stack = (char**)registers.esp;
	int32_t fd = (int32_t)stack[3];
	char* buffer = (char*)stack[2];
	size_t offset = (size_t)stack[1];
	size_t amount = (size_t)stack[0];

	Inode* inode = current_thread->get_inode(fd);
	if(!inode)
		return -1;

	File* file = inode->as_file();
	if(!file)
		return -1;

	return file->read(buffer, offset, amount);
}

int32_t syscall_write_fd(Registers& registers){
	char** stack = (char**)registers.esp;
	int32_t fd = (int32_t)stack[3];
	char* buffer = (char*)stack[2];
	size_t offset = (size_t)stack[1];
	size_t amount = (size_t)stack[0];

	Inode* inode = current_thread->get_inode(fd);
	if(!inode)
		return -1;

	File* file = inode->as_file();
	if(!file)
		return -1;

	return file->write(buffer, offset, amount);
}

int32_t syscall_wait(Registers& registers){
	char**stack = (char**)registers.esp;
	int tid = (int)stack[0];

	if (tid == current_thread->get_tid())
		return 0;

	if(Thread* waiting_on = Thread::lookup(tid)){
		return current_thread->wait_on_list<WaitBlocker>(waiting_on->waiters);
	}
	return -1;
}

void Thread::initialize(){
	//We set up the kernel thread to be the current running thread.
	current_thread = &kernel_thread;
	current_thread->set_state(ThreadState::Running);
	current_thread->set_remaining_ticks(current_thread->get_default_ticks());
	current_thread->set_pdir(kernel_page_directory());

	register_system_call(syscall_create_thread, SC_create_thread);
	register_system_call(syscall_exit_thread, SC_exit_thread);
	register_system_call(syscall_open_file, SC_open_file);
	register_system_call(syscall_close_file, SC_close_file);
	register_system_call(syscall_read_fd, SC_read);
	register_system_call(syscall_write_fd, SC_write);
	register_system_call(syscall_wait, SC_wait);
	register_system_call(syscall_signal, SC_signal);
	register_system_call(syscall_kill, SC_kill);
	register_interrupt_callback(tick_callback, 0x20);
}
