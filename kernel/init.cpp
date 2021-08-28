#include <icxxabi.h>
#include <unique.h>
#include <shared.h>
#include <multiboot.h>
#include <Thread.h>
#include <devices/IO.h>
#include <devices/Serial.h>
#include <devices/CPU.h>
#include <devices/Interrupts.h>
#include <devices/Keyboard.h>
#include <devices/TTY.h>
#include <devices/Clock.h>
#include <devices/IDE.h>
#include <memory/Paging.h>
#include <Malloc.h>
#include <filesystem/FileSystem.h>
#include <string.h>
#include <Lock.h>
#include <functional.h>

Thread* userspace_thread = nullptr;

VGATerminal* terminal = nullptr;

int apply(Func<int, int> f, int i){
	return f(i);
}

void writeHD(void){
	IO::out8(0x1f6, 0xE0);
	IO::out8(0x1f1,0);
	IO::out8(0x1f2, 1);
	IO::out8(0x1f3, 0);
	IO::out8(0x1f4, 0);
	IO::out8(0x1f5, 0);

	//write sectors command
	IO::out8(0x1f7, 0x30);

	//wait
	//TODO
	while(0 != (IO::in8(0x1f7) & 0b10000000)){
	}

	for(int i = 0; i < 256; i++){
		while(0 != (IO::in8(0x1f7) & 0b10000000)){
		}
		IO::out16(0x1f0, 0xbeef);
		//com1() << "in = " << (void*) IO::in16(0x1f0) << "\n";
	}

	while(0 != (IO::in8(0x1f7) & 0b10000000)){
	}

	IO::out8(0x1f7, 0xE7);


}
extern "C" int kernel_main(multiboot_info_t* info){
	multiboot_module_t *mods = (multiboot_module_t*)info->mods_addr;

	initialize_gdt_table();
	initialize_interrupts();
	initialize_paging(mods);
	Clock::initialize();
	com1() << "greetings\n";
	Thread::initialize();

	//load the initramfs module into the filesystem
	uint32_t *file_header = (uint32_t*)mods->mod_start;
	com1() << "busmaster: " << (void*)PCI::find_busmaster() << "\n";
	writeHD();

	initialize_file_system(file_header);

	terminal = new VGATerminal();
	initialize_keyboard(terminal);
	terminal->clear();

	PCI::check_devices();

	if(File* init_file = root_directory().lookup_file("vfs/init.elf")){
		userspace_thread = new Thread(*init_file, 0, nullptr, "");
	}

	root_directory().add_entry("console", terminal);

	{
		Unique<int> i = new int(10);
		Shared<int> is = new int(20);
		com1() << "howdy: " << *i << "\n"; 
		com1() << "howdy: " << *is << "\n"; 
	}

	com1() << "some test " << apply([](int i) { return i + 1;}, 10) << "\n";

	// ignore any SIGCHLDS.
	auto new_handler = SignalHandler();
	new_handler.set_disposition(SignalDisposition::Ignore);
	current_thread->set_handler(SIGCHLD, new_handler);

	// From this point onwards other threads may be scheduled
	sti();

	// This the idle loop for the whole system and is only run
	// when there are no runnable threads.
	while(true)
		asm("hlt");

	return 0;
}
