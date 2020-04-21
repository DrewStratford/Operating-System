#include <icxxabi.h>
#include <multiboot.h>
#include <Thread.h>
#include <devices/IO.h>
#include <devices/Serial.h>
#include <devices/CPU.h>
#include <devices/Interrupts.h>
#include <memory/Paging.h>
#include <memory/Heap.h>
#include <filesystem/FileSystem.h>
#include <string.h>
#include <Lock.h>

extern "C" int kernel_main(multiboot_info_t* info){
	initialize_gdt_table();
	initialize_interrupts();
	initialize_paging();
	com1() << "greetings\n";
	Thread::initialize();

	//load the initramfs module into the filesystem
	multiboot_module_t *mods = (multiboot_module_t*)info->mods_addr;
	uint32_t *file_header = (uint32_t*)mods->mod_start;

	initialize_file_system(file_header);

	Thread userspace_thread(root_directory().lookup_file("vfs/test.prog"));
	Thread userspace2(root_directory().lookup_file("vfs/test.prog"));
	
	sti();
	while(true){
		asm("hlt");
	}
	return 0;
}
