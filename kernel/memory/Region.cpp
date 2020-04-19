#include <memory/Region.h>
#include <devices/Serial.h>
#include <devices/CPU.h>


Region::Region(char* description, uint32_t start, uint32_t size) 
	: description(description), start(start), size(size){
}
Region::Region(uint32_t start, uint32_t size) : start(start), size(size){
}

const uint32_t Region::end() const {
	return start + size;
}

bool Region::contains(uintptr_t addr){
	return addr >= start && addr < end();
}

void Region::handle_page_fault(PageFaultType fault_type, uintptr_t addr){
	com1() << *this << " handling fault " << (void*)addr << "\n";

	switch(fault_type){
		case KernelWriteNP:
		case KernelReadNP:
			map_page((uintptr_t)addr, false);
			break;
		case UserWriteNP:
		case UserReadNP:
			panic("paging: userspace fault\n");
			break;
		default:
			panic("paging: protection fault\n");
			break;
	}
}

Serial& operator<<(Serial& serial, Region const& reg){
	serial << "Region: " << reg.get_description() << ", start: " 
			<< (void*)reg.get_start() << ", end: " << (void*)reg.end();
}
