#include <icxxabi.h>
#include <multiboot.h>
#include <Thread.h>
#include <devices/IO.h>
#include <devices/Serial.h>
#include <devices/CPU.h>
#include <devices/Interrupts.h>
#include <devices/Keyboard.h>
#include <devices/TTY.h>
#include <devices/Clock.h>
#include <memory/Paging.h>
#include <Malloc.h>
#include <filesystem/FileSystem.h>
#include <string.h>
#include <Lock.h>

Thread* userspace_thread = nullptr;

VGATerminal* terminal = nullptr;

extern "C" int kernel_main(multiboot_info_t* info){
	initialize_gdt_table();
	initialize_interrupts();
	initialize_paging();
	Clock::initialize();
	com1() << "greetings\n";
	Thread::initialize();

	//load the initramfs module into the filesystem
	multiboot_module_t *mods = (multiboot_module_t*)info->mods_addr;
	uint32_t *file_header = (uint32_t*)mods->mod_start;

	initialize_file_system(file_header);

	terminal = new VGATerminal();
	initialize_keyboard(terminal);
	terminal->clear();

	if(File* init_file = root_directory().lookup_file("vfs/init.prog")){
		userspace_thread = new Thread(*init_file, 0, nullptr, "");
	}

	root_directory().add_entry("console", terminal);

	// From this point onwards other threads may be scheduled
	sti();

	// This the idle loop for the whole system and is only run
	// when there are no runnable threads.
	while(true)
		asm("hlt");

	return 0;
}
