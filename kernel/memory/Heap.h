#pragma once

#include <stddef.h>
#include <stdint.h>


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
