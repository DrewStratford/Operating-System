#pragma once

#include <data/List.h>

class Blocker;
class Thread;

class Lock {
	int nesting { 0 };
	Thread* owner = nullptr;

public:
	void lock();
	void unlock();
	bool try_lock();

	Thread* get_owner() { return owner; }
};

class ScopedLocker{
private:
	Lock *lock;
public:
	ScopedLocker(Lock *lock);
	~ScopedLocker();
};
