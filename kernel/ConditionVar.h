#pragma once

#include <data/List.h>

class Blocker;

class ConditionVar {
public:
	void wake();
	void wait();
private:
	List<Blocker> m_wait_list;
};
