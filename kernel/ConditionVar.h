#pragma once

#include <Lock.h>
#include <data/List.h>

class Blocker;

class ConditionVar {
public:
	void wake();
	void wake_all();
	bool wait(Lock& lock);
private:
	Lock internal_lock;
	List<Blocker> m_wait_list;
};
