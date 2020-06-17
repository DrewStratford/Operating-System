#include <ConditionVar.h>
#include <Thread.h>

#include <devices/CPU.h>

void ConditionVar::wake(){
	NoInterrupts ni;
	Thread::wake_from_list(m_wait_list);
}

void ConditionVar::wake_all(){
	NoInterrupts ni;
	while(!m_wait_list.is_empty()){
		Thread::wake_from_list(m_wait_list);
	}
}

int ConditionVar::wait(Lock& lock){
	NoInterrupts ni;

	if(Thread::get_current() != lock.get_owner())
		return -1;

	lock.unlock();
	current_thread->wait_on_list(m_wait_list);
	lock.lock();
	return 0;
}
