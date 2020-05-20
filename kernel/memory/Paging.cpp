#include <memory/Paging.h>
#include <memory/Region.h>
#include <memory/Heap.h>

#include <string.h>

#include <Thread.h>
#include <devices/CPU.h>
#include <devices/Interrupts.h>
#include <devices/Serial.h>

alignas(0x1000) PTE init_page_directory[1024];
alignas(0x1000) PTE init_page_table[1024];

List<Region> kernel_regions;
List<Region>* user_regions = nullptr;

extern uintptr_t kernel_text_end, kernel_text, kernel_rodata, kernel_rodata_end, kernel_data, kernel_data_end, kernel_bss, kernel_bss_end, kernel_end;

GuardRegion null_region((uintptr_t)0, (uintptr_t)0x1000);
KernelRegion text_region("k_text", (uintptr_t)&kernel_text, (uintptr_t)&kernel_text_end - (uintptr_t)&kernel_text);
KernelRegion rodata_region("k_rodata", (uintptr_t)&kernel_rodata, (uintptr_t)&kernel_rodata_end - (uintptr_t)&kernel_rodata);
KernelRegion data_region("k_data", (uintptr_t)&kernel_data, (uintptr_t)&kernel_data_end - (uintptr_t)&kernel_data);
KernelRegion bss_region("k_bss", (uintptr_t)&kernel_bss, (uintptr_t)&kernel_bss_end - (uintptr_t)&kernel_bss);
KernelRegion heap_region("k_heap", (uintptr_t)&kernel_end, 0x40000000-(uintptr_t)&kernel_end);

static void create_page_table(uintptr_t address, bool is_userspace);


PTE* kernel_page_directory(){
	return init_page_directory;
}

List<Region>* get_user_regions(){
	return user_regions;
}

void set_user_regions(List<Region>* regions){
	user_regions = regions;
}

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

void reload_cr3(){
	asm("mov %%cr3, %%eax\n"
		"mov %%eax, %%cr3\n"
		::: "eax", "memory");
}

static void page_callback(Registers& registers){
	PageFaultType fault_type = (PageFaultType)registers.err;
	uintptr_t address = 0;
	asm("mov %%cr2, %[addr]\n" : [addr]"=mr"(address));

	for (Region* region = kernel_regions.peek(); region != nullptr; region = region->next){
		if(region->contains(address)){
			region->handle_page_fault(fault_type, address);
			return;
		}
	}

	Region* r = user_regions ? user_regions->peek() : nullptr;
	for (Region* region = r; region != nullptr; region = region->next){
		if(region->contains(address)){
			region->handle_page_fault(fault_type, address);
			return;
		}
	}
	panic("page_callback: page fault out of region\n");
}

void initialize_paging(){
	kernel_regions.insert(&null_region);
	kernel_regions.insert(&text_region);
	kernel_regions.insert(&rodata_region);
	kernel_regions.insert(&data_region);
	kernel_regions.insert(&bss_region);
	kernel_regions.insert(&heap_region);

	for (Region* region = kernel_regions.peek(); region != nullptr; region = region->next){
		com1() << *region << "\n";
	}

	//Note: we miss the first page as it is the NULL page.
	for(int i = 1; i < 1024; i++){
		PTE& pte = init_page_table[i];
		pte.set_address(i * 0x1000);
		pte.userspace = true;
		pte.writable = true;
		pte.present = true;
	}

	// Setup first page table
	PTE& page_directory = init_page_directory[0];
	page_directory.set_address(reinterpret_cast<uintptr_t>(init_page_table));
	page_directory.userspace = true;
	page_directory.writable = true;
	page_directory.present = true;

	// Setup last table as a recursive mapping
	PTE& recursive_mapping = init_page_directory[1023];
	recursive_mapping.set_address(reinterpret_cast<uintptr_t>(init_page_directory));
	recursive_mapping.userspace = true;
	recursive_mapping.writable = true;
	recursive_mapping.present = true;

	enable_paging(reinterpret_cast<uintptr_t>(init_page_directory));

	// Map in the first 1 GiB of page tables for use by the kernel.
	// We do this now to ensure that kerenel page entries stay consistent
	// across all processes.
	for(size_t i = 1; i < 256; i++)
		create_page_table(i * 1024 * 0x1000, false);

	register_interrupt_callback(page_callback, 0x0e);
	com1() << "finished paging\n";
	initialize_heap(heap_region.get_start(), heap_region.end());
}

static char* pagefaulttype_strs[] = {
	"KernelReadNP", "KernelReadPF", "KernelWriteNP", "KernelWritePF",
	"UserReadNP", "UserReadPF", "UserWriteNP", "UserWritePF"
};

OutStream& operator<<(OutStream& stream, PageFaultType err_code){
	stream << pagefaulttype_strs[err_code];
	return stream;
}

static PTE* get_recursive_table(uintptr_t address){
	uintptr_t index = 0x400 * (address >> 22);
	PTE* range = (PTE*)0xFFC00000;
	return &range[index];
}

static PTE* get_recursive_directory(){
	return (PTE*)0xFFFFF000;
}

PTE& lookup_page_table(uintptr_t address){
	uintptr_t index = address >> 22;
	return get_recursive_directory()[index];
}

PTE& lookup_page_entry(uintptr_t address){
	uintptr_t entry_index = (address >> 12) & 0x3ff;

	PTE* page_table = get_recursive_table(address);
	return page_table[entry_index];
}

uintptr_t v_to_p(uintptr_t address){
	if(!get_recursive_table(address))
		return (uintptr_t) nullptr;
	return lookup_page_entry(address).get_address();
}

static void create_page_table(uintptr_t address, bool is_userspace){
	uintptr_t table_physical = (uintptr_t)allocate_physical_page();
	PTE& entry = lookup_page_table(address);

	entry.set_address(table_physical);
	entry.userspace = is_userspace;
	entry.writable = true;
	entry.present = true;

	// It's important that we properly initialize the table.
	PTE* page_table = get_recursive_table(address);
	for(int i = 0; i < 1024; i++)
		page_table[i] = PTE();
}

// Maps a virtual address to a physical page.
bool map_page(uintptr_t address, bool is_userspace){
	PTE& table = lookup_page_table(address);

	if(!table.present)
		create_page_table(address, is_userspace);

	PTE& entry = lookup_page_entry(address);

	if(entry.present)
		return false;

	uintptr_t entry_physical = (uintptr_t)allocate_physical_page();
	entry.set_address(entry_physical);
	entry.userspace = is_userspace;
	entry.writable = true;
	entry.present = true;
	com1() << "mapped: " << (void*)address << " -> " << (void*)entry_physical << "\n";
	return true;
}

void initialize_page_directory(PTE* directory){
	PTE* old_directory = get_recursive_directory();
	memset(directory, 0, 0x1000);

	//copy over the kernel page tables, which are consistent across all processes
	for(size_t i = 0; i < 256; i++)
		directory[i] = old_directory[i];

	PTE& recursive_mapping = directory[1023];
	recursive_mapping.set_address(reinterpret_cast<uintptr_t>(v_to_p((uintptr_t)directory)));
	recursive_mapping.userspace = true;
	recursive_mapping.writable = true;
	recursive_mapping.present = true;
}

// For the mean time, we just use a watermark allocator.
// This is exceedingly simple, but lacks the abillity to
// free pages.
void* allocate_physical_page(){
	static void* level = (void*)(0x400 * 0x1000);
	void* out = level;
	level += 0x1000;
	return out;
}

void free_physical_page(void* page){
	// Do nothing.
}
