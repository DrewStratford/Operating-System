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

bool ConditionVar::wait(Lock& lock){

	if(Thread::get_current() != lock.get_owner())
		return false;

	lock.unlock();
	bool interrupted;
	current_thread->wait_on_list<Blocker>(m_wait_list, internal_lock, &interrupted);
	lock.lock();
	return !interrupted;
}
