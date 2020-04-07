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

void* kmemalign(size_t alignment, size_t size){
	uintptr_t offset = water_mark % alignment;
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
