#include <memory/Heap.h>

static uintptr_t water_mark = 0;
static uintptr_t water_mark_end = 0;

void initialize_heap(uintptr_t start, uintptr_t end){
	water_mark = start;
	water_mark_end = end;
}


void* kmalloc(size_t size){
	void* out = (void*)water_mark;
	water_mark += size;
	return out;
}

void* krealloc(void* ptr, size_t size){
	if(ptr == nullptr)
		return kmalloc(size);

	uint8_t* new_ptr = (uint8_t*)kmalloc(size);
	uint8_t* old_ptr = (uint8_t*)ptr;
	for(int i = 0; i < size; i++)
		new_ptr[i] = old_ptr[i];
	return new_ptr;
}

void* kreallocarray(void* ptr, size_t members, size_t size){
	return krealloc(ptr, members * size);
}

void* kmemalign(size_t alignment, size_t size){
	uintptr_t offset = alignment - (water_mark % alignment);
	water_mark += offset;
	return kmalloc(size);
}

void kfree(void* ptr){
	// Do nothing
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
