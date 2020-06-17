#pragma once

#include <Lock.h>
#include <data/List.h>

class Blocker;

class ConditionVar {
public:
	void wake();
	void wake_all();
	int wait(Lock& lock);
private:
	List<Blocker> m_wait_list;
};
