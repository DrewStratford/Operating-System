#include  <SyscallInfo.h>

int debug(char* cs){
	asm("push %[msg]\n"
		"mov %[no], %%eax\n"
		"int $0x80\n"
		"add $0x04, %%esp\n"
		:: [no]"i"(SC_debug), [msg]"irm"(cs));
	return 0;
}

int create_thread(char* cs){
	asm("push %[msg]\n"
		"mov %[no], %%eax\n"
		"int $0x80\n"
		"add $0x04, %%esp\n"
		:: [no]"i"(SC_create_thread), [msg]"irm"(cs));
	return 0;
}

int exit(){
	asm("mov %[no], %%eax\n"
		"int $0x80\n"
		:: [no]"i"(SC_exit_thread));
	return 0;
}

int main(void){
	debug("-> INIT\n");	
	create_thread("vfs/test.prog");
	debug("-> created thread\n");	
	exit();
	return 0;
}

