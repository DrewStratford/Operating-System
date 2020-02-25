#pragma once

template <typename T>
class ListNode {

private:
	T& value;
	ListNode<T>* previous;
	ListNode<T>* next;
};
