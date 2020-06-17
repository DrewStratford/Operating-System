#pragma once

#include <data/List.h>

class Blocker;
class Thread;

class Lock {
	Thread * owner { nullptr };
	List<Blocker> wait_list;

public:
	void lock();
	void unlock();

	Thread* get_owner() { return owner; }
};

class ScopedLocker{
private:
	Lock *lock;
public:
	ScopedLocker(Lock *lock);
	~ScopedLocker();
};
