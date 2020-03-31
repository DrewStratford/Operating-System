#pragma once

#include <stdint.h>

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

constexpr uintptr_t present = 0b0001;
constexpr uintptr_t write = 0b0010;
void initialize_paging();
