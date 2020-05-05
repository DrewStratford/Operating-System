#include <ConditionVar.h>
#include <Thread.h>

#include <devices/CPU.h>

void ConditionVar::wake(){
	NoInterrupts ni;
	Thread::wake_from_list(m_wait_list);
}

void ConditionVar::wait(){
	NoInterrupts ni;
	current_thread->wait_on_list(m_wait_list);
}
