#pragma once

#include <data/List.h>
#include <Thread.h>

class Lock {
	Thread * owner { nullptr };
	List<Blocker> wait_list;

public:
	void lock();
	void unlock();
};

class ScopedLocker{
private:
	Lock *lock;
public:
	ScopedLocker(Lock *lock);
	~ScopedLocker();
};