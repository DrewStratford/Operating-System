#include <icxxabi.h>
#include <unique.h>
#include <shared.h>
#include <multiboot.h>
#include <Thread.h>
#include <devices/ATA.h>
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
ATA* ata_disk = nullptr;

int apply(Func<int, int> f, int i){
	return f(i);
}

static void ata_dma_thread(){
	sti();
	uint8_t* buffer = new uint8_t[1024];
	memset(buffer, 0, 1024);

	ata_disk->read_dma(0, 1024, buffer);
	ata_disk->write_dma(3, 1024, buffer);
	ata_disk->read_dma(1, 1024, buffer);

	current_thread->mark_for_death(0);
	while(true)
		asm("hlt");
}

extern "C" int kernel_main(multiboot_info_t* info){
	multiboot_module_t *mods = (multiboot_module_t*)info->mods_addr;

	initialize_gdt_table();
	initialize_interrupts();
	initialize_paging(mods);
	Clock::initialize();
	com1() << "greetings\n";
	Thread::initialize();
	ata_disk = ATA::initialize(PCI::find_busmaster());

	//load the initramfs module into the filesystem
	uint32_t *file_header = (uint32_t*)mods->mod_start;

	initialize_file_system(file_header);

	terminal = new VGATerminal();
	initialize_keyboard(terminal);
	terminal->clear();

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

	// Test disk dma in new thread.
	char* dma_stack = new char[2000] + 2000;
	Thread* interupt_thread =
		new Thread((uintptr_t)dma_stack, (uintptr_t)ata_dma_thread);

	// This the idle loop for the whole system and is only run
	// when there are no runnable threads.
	while(true)
		asm("hlt");

	return 0;
}
