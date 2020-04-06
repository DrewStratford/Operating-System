#pragma once

#include <devices/Serial.h>

#include <data/List.h>
#include <memory/Paging.h>

class Region : public ListNode<Region>{
public:
	Region(char* description, uintptr_t start, uintptr_t end);
	Region(uintptr_t start, uintptr_t end);
	const uintptr_t get_start() const { return start; };
	char* get_description() const { return description; };
	const uintptr_t end() const ;
	bool contains(uintptr_t);
	void handle_page_fault(PageFaultType, uintptr_t addr);

private:
	uintptr_t start { 0 };
	uintptr_t size { 0 };
	char* description { "anonymous region" };
};

Serial& operator<<(Serial& serial, Region const& region);
