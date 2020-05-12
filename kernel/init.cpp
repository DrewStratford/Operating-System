#include <icxxabi.h>
#include <multiboot.h>
#include <Thread.h>
#include <devices/IO.h>
#include <devices/Serial.h>
#include <devices/CPU.h>
#include <devices/Interrupts.h>
#include <devices/Keyboard.h>
#include <memory/Paging.h>
#include <memory/Heap.h>
#include <filesystem/FileSystem.h>
#include <string.h>
#include <Lock.h>

void foo2(void){
	sti();
	while(true){
		asm("hlt");
	}
}
void foo(void){
	File& f = keyboard_file();
	char buffer[11];

	while(true){
		int c = f.read(buffer, 0, 10);
		buffer[c] = '\0';
		com1() << "{" << buffer << "}\n";
	}
}

char foo_stack[1000];
char foo_stack2[1000];

Thread* userspace_thread = nullptr;

extern "C" int kernel_main(multiboot_info_t* info){
	initialize_gdt_table();
	initialize_interrupts();
	initialize_paging();
	initialize_keyboard();
	com1() << "greetings\n";
	Thread::initialize();

	//load the initramfs module into the filesystem
	multiboot_module_t *mods = (multiboot_module_t*)info->mods_addr;
	uint32_t *file_header = (uint32_t*)mods->mod_start;

	initialize_file_system(file_header);

	if(File* init_file = root_directory().lookup_file("vfs/init.prog")){
		userspace_thread = new Thread(init_file);
	}

	Thread key_thread((uintptr_t)&foo_stack[999], (uintptr_t)foo);
	Thread thread2((uintptr_t)&foo_stack2[999], (uintptr_t)foo2);
	

	sti();
	while(true){
		asm("hlt");
	}
	return 0;
}
