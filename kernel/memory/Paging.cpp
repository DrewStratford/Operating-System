#include <memory/Paging.h>

#include <devices/CPU.h>
#include <devices/Interrupts.h>
#include <devices/Serial.h>

alignas(0x1000) PTE init_page_directory[1024];
alignas(0x1000) PTE init_page_table[1024];

void load_cr3(uintptr_t cr3){
	asm("mov %[cr3], %%cr3\n"
		:: [cr3]"r"(cr3)
		: "memory");
}

void enable_paging(uintptr_t cr3){
	asm("mov %[cr3], %%cr3\n"

		// enable global pages
		"mov %%cr4, %%eax\n"
		"or $0x80, %%eax\n"
		"mov %%eax, %%cr4\n"

		//enable paging
		"mov %%cr0, %%eax\n"
		"or $0x80000000, %%eax\n"
		"mov %%eax, %%cr0\n"

		:: [cr3]"r"(cr3)
		: "eax", "memory");
}

static void page_callback(Registers& registers){
	panic("page_callback\n");
}

void initialize_paging(){
	com1() << "initializing paging\n";

	for(int i = 0; i < 1024; i++){
		PTE& pte = init_page_table[i];
		pte.set_address(i * 0x1000);
		pte.writable = true;
		pte.present = true;
	}

	PTE& page_directory = init_page_directory[0];
	page_directory.set_address(reinterpret_cast<uintptr_t>(init_page_table));
	page_directory.writable = true;
	page_directory.present = true;
	enable_paging(reinterpret_cast<uintptr_t>(init_page_directory));

	register_interrupt_callback(page_callback, 0x0e);
	com1() << "finished paging\n";
}
