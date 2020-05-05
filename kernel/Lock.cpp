#include <Lock.h>
#include <devices/CPU.h>

#include <Thread.h>


void Lock::lock(){
	NoInterrupts ni;

	while(owner != nullptr){
		current_thread->wait_on_list(wait_list);	
	}
	owner = current_thread;
}

void Lock::unlock(){
	NoInterrupts ni;

	owner = nullptr;
	Thread::wake_from_list(wait_list);
}

ScopedLocker::ScopedLocker(Lock *lock){
	this->lock = lock;
	lock->lock();
}

ScopedLocker::~ScopedLocker(){
	lock->unlock();
}
