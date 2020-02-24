#pragma once

#include <stdint.h>
class Thread {
public:
	Thread(uintptr_t stack, uintptr_t resume);
	static void switch_thread(Thread& from, Thread& to);

private:
	uintptr_t stack_ptr { 0 };
	uintptr_t resume_ptr { 0 };
};
