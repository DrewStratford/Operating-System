#include <Lock.h>
#include <devices/CPU.h>

#include <Thread.h>


void Lock::lock(){

	while(owner != current_thread
		&& !__sync_bool_compare_and_swap(&owner, nullptr, current_thread)){
		Thread::yield();
	}
	owner = current_thread;
	nesting++;
}

bool Lock::try_lock(){

	if(owner == current_thread
		|| __sync_bool_compare_and_swap(&owner, nullptr, current_thread)){

		owner = current_thread;
		nesting++;
		return true;
	}
	return false;

}

void Lock::unlock(){
	nesting--;
	if(nesting == 0)
		owner = nullptr;
}

ScopedLocker::ScopedLocker(Lock *lock){
	this->lock = lock;
	lock->lock();
}

ScopedLocker::~ScopedLocker(){
	lock->unlock();
}
