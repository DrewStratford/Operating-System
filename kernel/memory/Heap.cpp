#include <memory/Heap.h>
#include <devices/Serial.h>
#include <devices/CPU.h>

FreeNode* free_nodes = nullptr;
FreeNode end_node = FreeNode(0);
FreeNode start_node = FreeNode(0);

void initialize_heap(uintptr_t start, uintptr_t end){

	size_t heap_size = end - start - sizeof(FreeNode);
	FreeNode* allocation = (FreeNode*)start;
	*allocation = FreeNode(heap_size);

	start_node.m_next = allocation;
	allocation->m_previous = &start_node;

	allocation->m_next = &end_node;
	end_node.m_previous = allocation;

	free_nodes = &start_node;
}


void* kmalloc(size_t size){
	if(size < sizeof(FreeNode))
		size = sizeof(FreeNode);

	for(FreeNode* node = free_nodes; node != nullptr; node = node->m_next){
		if(node->split(size)){
			node->remove();

			if(node->magic == 0xbeef)
				panic("already 0xbeef");

			node->magic = 0xbeef;
			return (void*)node->start();
		}
	}
	return nullptr;
}

void* krealloc(void* ptr, size_t size){
	if(ptr == nullptr)
		return kmalloc(size);

	FreeNode* ptr_info = (FreeNode*)(ptr - sizeof(FreeNode));

	uint8_t* new_ptr = (uint8_t*)kmalloc(size);
	uint8_t* old_ptr = (uint8_t*)ptr;
	for(int i = 0; i < ptr_info->m_size; i++)
		new_ptr[i] = old_ptr[i];

	kfree(old_ptr);
	return new_ptr;
}

void* kreallocarray(void* ptr, size_t members, size_t size){
	return krealloc(ptr, members * size);
}

void* kmemalign(size_t alignment, size_t size){
	for(FreeNode* node = free_nodes; node != nullptr; node = node->m_next){
		if(node->split_align(size, alignment)){
			node->remove();
			com1() << "kmemalign: " << (void*) node->start() << ", " << (void*)node->end() << ", " << (void*)node->magic << "\n";
			if(node->magic == 0xbeef){
				panic("already 0xbeef");
			}
			node->magic = 0xbeef;
			return (void*)node->start();
		}
	}
	return nullptr;
}

void kfree(void* ptr){
	FreeNode* free = (FreeNode*)(ptr - sizeof(FreeNode));

	if(free->magic == 0xfeed)
		panic("kfree: freeing free memory!");

	if(free->magic != 0xbeef){
		com1() << "kfree: trying to free unmanaged memory!\n";
		return;
	}

	// Reinsert the node in the free list
	for(FreeNode* next = free_nodes; next != nullptr; next = next->m_next){
		if(free->start() < next->start()){
			auto tmp = next->m_previous;
			free->m_next = next;
			free->m_previous = tmp;

			next->m_previous = free;
			tmp->m_next = free;
			free->magic = 0xfeed;
			return;
		}
	}
	com1() << "couldn't free " << ptr << "\n";
	panic("");
}

void* operator new(size_t size){
	return kmalloc(size);
}

void* operator new[](size_t size){
	return kmalloc(size);
}

void operator delete(void* ptr){
	kfree(ptr);
}

void operator delete(void* ptr, size_t size){
	kfree(ptr);
}


void operator delete[](void* ptr, size_t size){
	kfree(ptr);
}
