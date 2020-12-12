#pragma once
#include <data/Vector.h>

#include <devices/CPU.h>
#include <ConditionVar.h>

// This is a very specific queue, intended
// only to be used when handling interrupts.
// 
// It should only be written to by an interrupt handler stub;
// and read only by an interrupt handling thread with interrupts
// disabled.

template<typename T>
class InterruptQueue{
private:
	Vector<T> m_data;
	ConditionVar waiters;

public:
	void append(T& t){
		m_data.insert_end(t);
		waiters.wake();
	}

	T read(){
		NoInterrupts ni;
		while(m_data.is_empty()){
			Lock l;
			l.lock();
			waiters.wait(l);
			l.unlock();
		}
		return m_data.remove_front();		
	}
};
