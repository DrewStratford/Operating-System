#pragma once

#include <devices/Serial.h>

#include <data/List.h>
#include <memory/Paging.h>

class Region : public ListNode<Region>{
public:
	const uintptr_t get_start() const { return start; };
	char* get_description() const { return description; };
	const uintptr_t end() const ;
	bool contains(uintptr_t);
	virtual void handle_page_fault(PageFaultType, uintptr_t addr) = 0;

protected:
	Region(char* description, uintptr_t start, uintptr_t end);
private:
	uintptr_t start { 0 };
	uintptr_t size { 0 };
	char* description { "anonymous region" };
};

class KernelRegion : public Region {
public:
	KernelRegion(char* description, uintptr_t start, uintptr_t end);
	KernelRegion(uintptr_t start, uintptr_t end);

	virtual void handle_page_fault(PageFaultType, uintptr_t addr);
};

class UserRegion : public Region {
public:
	UserRegion(char* description, uintptr_t start, uintptr_t end);
	UserRegion(uintptr_t start, uintptr_t end);

	virtual void handle_page_fault(PageFaultType, uintptr_t addr);
};

class GuardRegion : public Region {
public:
	GuardRegion(uintptr_t start, uintptr_t end);
	virtual void handle_page_fault(PageFaultType, uintptr_t addr);
};

Serial& operator<<(Serial& serial, Region const& region);
