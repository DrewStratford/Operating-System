#include  <SyscallInfo.h>

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

int main(void){
	debug("hello from userspace");	
	exit();
	return 0;
}
