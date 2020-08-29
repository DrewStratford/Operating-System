#include <System.h>

int debug(char* cs){
	asm("push %[msg]\n"
		"mov %[no], %%eax\n"
		"int $0x80\n"
		"add $0x04, %%esp\n"
		:: [no]"i"(SC_debug), [msg]"irm"(cs));
	return 0;
}

int exit(int code){
	asm("push %[c]\n"
		"mov %[no], %%eax\n"
		"int $0x80\n"
		:: [c]"irm"(code), [no]"i"(SC_exit_thread));
	return 0;
}

int32_t get_time(){
	int32_t out = 0;
	asm("mov %[no], %%eax\n"
		"int $0x80\n"
		: "=a"(out)
		: [no]"i"(SC_timestamp));
	return out;
}

int open(char* cs){
	asm("push %[path]\n"
		"mov %[no], %%eax\n"
		"int $0x80\n"
		"add $0x04, %%esp\n"
		:: [no]"i"(SC_open_file), [path]"irm"(cs));
	return 0;
}

int close(int fd){
	asm("push %[fd]\n"
		"mov %[no], %%eax\n"
		"int $0x80\n"
		"add $0x04, %%esp\n"
		:: [no]"i"(SC_close_file), [fd]"irm"(fd));
	return 0;
}

int write(int fd, void* buffer, size_t offset, size_t count){
	int32_t out = 0;
	asm("push %[fd]\n"
		"push %[buf]\n"
		"push %[off]\n"
		"push %[size]\n"
		"mov %[no], %%eax\n"
		"int $0x80\n"
		"add $0x10, %%esp\n"
		: "=a"(out)
		: [no]"i"(SC_write),
		  [fd]"irm"(fd),
		  [buf]"irm"(buffer),
		  [off]"irm"(offset),
		  [size]"irm"(count));
	return out;
}

int read(int fd, void* buffer, size_t offset, size_t count){
	int32_t out = 0;
	asm("push %[fd]\n"
		"push %[buf]\n"
		"push %[off]\n"
		"push %[size]\n"
		"mov %[no], %%eax\n"
		"int $0x80\n"
		"add $0x10, %%esp\n"
		: "=a"(out)
		: [no]"i"(SC_read),
		  [fd]"irm"(fd),
		  [buf]"irm"(buffer),
		  [off]"irm"(offset),
		  [size]"irm"(count));
	return out;
}

int create_thread(char* cs){
	return create_thread(cs, 0, nullptr);
}

int create_thread(char* cs, size_t inode_count, int* inodes){
	int out = 0;
	asm("push %[is]\n"
		"push %[icount]\n"
		"push %[path]\n"
		"mov %[no], %%eax\n"
		"int $0x80\n"
		"add $0x0c, %%esp\n"
		: "=a"(out)
		: [no]"i"(SC_create_thread),
		  [path]"irm"(cs),
		  [icount]"irm"(inode_count),
		  [is]"irm"(inodes));
	return out;
}

int wait(int thread_id){
	int32_t out = 0;
	asm("push %[tid]\n"
		"mov %[no], %%eax\n"
		"int $0x80\n"
		"add $0x04, %%esp\n"
		: "=a"(out)
		: [no]"i"(SC_wait),
		  [tid]"irm"(thread_id));
	return out;
}

int kill(int tid, int signal){
	int32_t out = 0;
	asm("push %[tid]\n"
		"push %[sig]\n"
		"mov %[no], %%eax\n"
		"int $0x80\n"
		"add $0x04, %%esp\n"
		: "=a"(out)
		: [no]"i"(SC_kill),
		  [sig]"irm"(signal),
		  [tid]"irm"(tid));
	return out;
}

int signal(int signal, uintptr_t handler){
	int32_t out = 0;
	asm("push %[hndlr]\n"
		"push %[sig]\n"
		"mov %[no], %%eax\n"
		"int $0x80\n"
		"add $0x04, %%esp\n"
		: "=a"(out)
		: [no]"i"(SC_signal),
		  [sig]"irm"(signal),
		  [hndlr]"irm"(handler));
	return out;
}
