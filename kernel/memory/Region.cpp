#include <memory/Region.h>
#include <devices/Serial.h>
#include <devices/CPU.h>


Region::Region(char* description, uint32_t start, uint32_t size) 
	: description(description), start(start), size(size){
}

UserRegion::UserRegion(char* description, uint32_t start, uint32_t size)
	: Region(description, start, size) {
}

UserRegion::UserRegion(uint32_t start, uint32_t size)
	: Region("anonymous", start, size) {
}

KernelRegion::KernelRegion(char* description, uint32_t start, uint32_t size)
	: Region(description, start, size) {
}

KernelRegion::KernelRegion(uint32_t start, uint32_t size)
	: Region("anonymous", start, size) {
}

const uint32_t Region::end() const {
	return start + size;
}

bool Region::contains(uintptr_t addr){
	return addr >= start && addr < end();
}

void UserRegion::handle_page_fault(PageFaultType fault_type, uintptr_t addr){
	com1() << *this << " handling UserRegion fault " << (void*)addr << "\n";

	switch(fault_type){
		case KernelWriteNP:
		case KernelReadNP:
		case UserWriteNP:
		case UserReadNP:
			map_page((uintptr_t)addr, true);
			break;
		default:
			panic("paging: protection fault\n");
			break;
	}
}

void KernelRegion::handle_page_fault(PageFaultType fault_type, uintptr_t addr){
	com1() << *this << " handling KernelRegion fault " << (void*)addr << "\n";

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

GuardRegion::GuardRegion(uintptr_t start, uintptr_t end)
	: Region("GuardRegion", start, end) { }

void GuardRegion::handle_page_fault(PageFaultType fault_type, uintptr_t addr){
	com1() << "fault in GuardRegion: " << (void*)addr << "\n";
	panic("");
}

OutStream& operator<<(OutStream& stream, Region const& reg){
	stream << "Region: " << reg.get_description() << ", start: " 
			<< (void*)reg.get_start() << ", end: " << (void*)reg.end();
	return stream;
}
