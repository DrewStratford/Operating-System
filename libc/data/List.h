#pragma once

#include <cstddef>

template <typename T> class List;

template <typename T>
class ListNode {
public:
	void remove(){
		if(previous != nullptr)
			previous->next = next;
		if(next != nullptr)
			next->previous = previous;

		previous = nullptr;
		next = nullptr;
	}

	friend class List<T>;
	T* previous { nullptr };
	T* next { nullptr };
};

template <typename T>
class List {
public:
	void remove(T* removee){
		if(removee == head)
			head = removee->next;
		if(removee == tail)
			tail = removee->previous;
		removee->remove();
	}

	void insert(T* insertee){
		if(head == nullptr){
			head = insertee;
			tail = insertee;
		} else {
			insert_before(insertee, head);
			head = insertee;
		}
	}

	void insert_end(T* insertee){
		if(head == nullptr){
			head = insertee;
			tail = insertee;
		} else {
			insert_after(insertee, tail);
			tail = insertee;
		}
	}

	T* pop_end(){
		if(tail == nullptr)
			return nullptr;
		T* out = tail;
		tail = tail->previous;

		if(tail == nullptr)
			head = nullptr;

		out->remove();
		return out;
	}

	T* pop(){
		if(head == nullptr)
			return nullptr;
		T* out = head;
		head = out->next;

		if(head == nullptr)
			tail = nullptr;

		out->remove();
		return out;
	}

	T* peek_end(){
		return tail;
	}

	T* peek(){
		return head;
	}

	bool is_empty(){
		return head == nullptr;
	}

	void mapM_(auto (*f)(T& t) -> void){
		for (T* t = head; t != nullptr; t = t->next){
			f(*t);
		}
		return;
	}

private:
	static void insert_after(T* insertee, T* before){
		T* left = before->next;

		insertee->next = left;
		insertee->previous = before;
		before->next = insertee;

		if(left != nullptr)
			left->previous = insertee;
	}

	static void insert_before(T* insertee, T* after){
		T* right = after->previous;

		insertee->previous = right;
		insertee->next = after;
		after->previous = insertee;

		if(right != nullptr)
			right->next = insertee;
	}

	T* head { nullptr };
	T* tail { nullptr };
};
