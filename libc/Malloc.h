#pragma once

#include <stddef.h>
#include <stdint.h>

#include <data/List.h>

extern void panic(char*);

enum FreeNodeState{
	Invalid,
	Free,
	Allocated
};

class [[gnu::packed]] FreeNode{

public:
	FreeNodeState magic { Free };
	size_t m_size { 0 };
	FreeNode* m_next { nullptr };
	FreeNode* m_previous { nullptr };

	FreeNode(size_t size) : m_size(size){
	}

	size_t start(){
		return (size_t)this + sizeof(FreeNode);
	}

	size_t end(){
		return start() + m_size;
	}

	bool split(size_t size){
		if(size == m_size)
			return true;

		// Can't allocate in this node.
		if(m_size < size + sizeof(FreeNode))
			return false;

		// Create a new node at the split point
		FreeNode* new_node = (FreeNode*)(start() + size);
		*new_node = FreeNode(m_size - size - sizeof(FreeNode));

		auto* tmp = m_next;
		this->m_next = new_node;
		new_node->m_previous = this;
		new_node->m_next = tmp;
		tmp->m_previous = new_node;

		m_size = size;
		return true;
	}

	bool split_align(size_t size, size_t alignment){
		// We're aligned so can malloc
		if(start() % alignment == 0)
			return split(size);

		// TODO: this is hacky
		// Attempt to create a new split that has the proper alignment.
		// This split will always fail, but when malloc checks the next
		// node it should succeed.

		intptr_t next_alignment = (intptr_t)alignment - ((intptr_t)start() % (intptr_t)alignment);
		//intptr_t split_size = next_alignment - (intptr_t)sizeof(size_t) - (intptr_t)sizeof(int32_t);
		intptr_t split_size = next_alignment - (intptr_t)sizeof(FreeNode);

		// Can't suitably split this node.
		if(split_size < 0)
			return false;

		// This should create a new node with the right alignment.
		// However, we don't now whether it will actually fit!
		split((size_t)split_size);
		return false;
	}

	void remove(){
		if(m_next == nullptr || m_previous == nullptr)
			return;
		if(magic == Allocated)
			panic("removing already allocted node");

		m_previous->m_next = m_next;
		m_next->m_previous = m_previous;
		m_previous = nullptr;
		m_next = nullptr;
		magic = Allocated;
	}

	// Merges with the next node if appropriate
	void merge(){
		if(end() != (size_t)m_next)
			return;

		m_next->magic = Invalid;
		m_size += sizeof(FreeNode) + m_next->m_size;
		m_next = m_next->m_next;
		m_next->m_previous = this;
	}
};

void initialize_heap(uintptr_t start, uintptr_t end);

void* kmalloc(size_t size);
void* kmemalign(size_t alignment, size_t size);
void* krealloc(void* ptr, size_t size);
void* kreallocarray(void* ptr, size_t members, size_t size);
void kfree(void* ptr);

void* operator new(size_t size);
void* operator new[](size_t size);
void operator delete(void* ptr);
void operator delete(void* ptr, size_t size);
void operator delete[](void* ptr, size_t size);
