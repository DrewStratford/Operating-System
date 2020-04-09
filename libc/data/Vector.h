#pragma once

#include <devices/Serial.h>
#include <memory/Heap.h>
#include <cstddef>

template<typename T>
class Vector{

public:
	Vector(){
		m_data = (T*)kmalloc(sizeof(T) * m_capacity);
	}

	Vector(Vector& other){
		m_data = (T*)kmalloc(sizeof(T) * m_capacity);
		for(int i = 0; i < other.size(); i++){
			m_data[i] = other[i];
		}
	}

	~Vector(){
		for(int i = 0; i < m_length; i++){
			delete &m_data[i];
		}
	}

	void insert_end(T t){
		resize();
		m_data[m_length] = t;
		m_length++;
	}

	void insert_front(T t){
		insert(0, t);
	}

	void insert(size_t index, T t){
		resize();
		insert_impl(index);
		m_data[index] = t;
	}

	T remove(size_t index){
		T out = m_data[index];
		remove_impl(index);
		return out;
	}

	T remove_end(){
		size_t idx = m_length - 1;
		return remove(idx);
	}
	
	T remove_front(){
		return remove(0);
	}

	T& operator[] (size_t index){
		return m_data[index];
	}

	size_t size() { return m_length; }
	bool is_empty() { return m_length == 0; }

private:
	size_t m_length { 0 };
	size_t m_capacity { 10 };
	T* m_data { nullptr };

	void remove_impl(size_t index){
		for(int i = index+1; i < size(); i++)
			m_data[i-1] = m_data[i];
		m_length--;
	}

	void insert_impl(size_t index){
		for(int i = size(); i > index; i--){
			m_data[i] = m_data[i-1];
		}
		m_length++;
	}

	void resize(){
		if(m_length == m_capacity){
			m_capacity *= 2;
			T* old_data = m_data;
			m_data = (T*)krealloc(m_data, sizeof(T) * m_capacity);
		}
	}

};
