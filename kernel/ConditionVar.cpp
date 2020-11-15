#include <ConditionVar.h>
#include <Thread.h>

#include <devices/CPU.h>

void ConditionVar::wake(){
	Thread::wake_from_list(m_wait_list, internal_lock);
}

void ConditionVar::wake_all(){
	while(!m_wait_list.is_empty()){
		Thread::wake_from_list(m_wait_list, internal_lock);
	}
}

int ConditionVar::wait(Lock& lock){

	if(Thread::get_current() != lock.get_owner())
		return -1;

	lock.unlock();
	current_thread->wait_on_list<Blocker>(m_wait_list, internal_lock);
	lock.lock();
	return 0;
}
