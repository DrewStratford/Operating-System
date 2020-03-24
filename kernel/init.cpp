#include <icxxabi.h>
#include <Thread.h>
#include <devices/IO.h>
#include <devices/Serial.h>
#include <devices/CPU.h>
#include <string.h>

char foo_stack[500];
void foo(){
	for(int i = 0; i < 10; i++){
		com1().write_string("foo()\n");
		Thread::yield();
	}
	Thread::die();
}

extern "C" int kernel_main(){
	com1().write_string("greetings\n");
	Thread::initialize();
	Thread::yield();
	Thread t((uintptr_t)&foo_stack[499], (uintptr_t)foo);
	for(int i = 0; i < 20; i++){
		com1().write_string("kernel_thread\n");
		Thread::yield();
	}

	return 0;
}
