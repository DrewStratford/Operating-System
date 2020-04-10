#pragma once

#include <stdint.h>

#include <devices/Serial.h>

struct [[gnu::packed]] PTE{
	bool present : 1 { 0 };
	bool writable : 1 { 0 };
	bool userspace : 1 { 0 };
	bool pwt : 1 { 0 };
	bool pcd : 1 { 0 };
	bool accessed : 1 { 0 };
	bool dirty : 1 { 0 };
	bool pat : 1 { 0 };
	bool global : 1 { 0 };
	uint8_t ignored : 3 { 0 };
	uintptr_t address : 20 { 0 };

	void set_address(uintptr_t addr){
		address = addr >> 12;
	}

	uintptr_t get_address(){
		return this->address << 12;
	}
};

enum PageFaultType{
	KernelReadNP,
	KernelReadPF,
	KernelWriteNP,
	KernelWritePF,

	UserReadNP,
	UserReadPF,
	UserWriteNP,
	UserWritePF
};

Serial& operator<<(Serial& serial, PageFaultType err_code);

void initialize_paging();

PTE& lookup_page_table(uintptr_t address);
PTE& lookup_page_entry(uintptr_t address);
bool map_kernel_page(uintptr_t address);

void* allocate_physical_page();
void free_physical_page(void* page);