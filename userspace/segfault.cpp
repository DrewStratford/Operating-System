#include <Signal.h>
#include <System.h>
#include <string.h>
#include <FileStream.h>
#include <Time.h>
void handler(void){
	debug("in segfault signal handler\n");
	exit(-101);
}
int main(void){
	int stdfd = 0;
	FileStream stream(stdfd);
	signal(SIGSEGV, (uintptr_t)handler);
	stream << "About to cause a segfault\n";
	volatile char c = *(char*)0x0;
	stream << "yowza\n";
	return 0;
}

