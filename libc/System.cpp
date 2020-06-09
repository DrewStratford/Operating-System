#include <System.h>

int debug(char* cs){
	asm("push %[msg]\n"
		"mov %[no], %%eax\n"
		"int $0x80\n"
		"add $0x04, %%esp\n"
		:: [no]"i"(SC_debug), [msg]"irm"(cs));
	return 0;
}

int exit(){
	asm("mov %[no], %%eax\n"
		"int $0x80\n"
		:: [no]"i"(SC_exit_thread));
	return 0;
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
		: [out]"=rm"(out)
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
		: [out]"=rm"(out)
		: [no]"i"(SC_read), 
		  [fd]"irm"(fd),
		  [buf]"irm"(buffer),
		  [off]"irm"(offset),
		  [size]"irm"(count));
	return out;
}
