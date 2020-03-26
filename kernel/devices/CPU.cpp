#include <devices/CPU.h>
#include <devices/Serial.h>


GDTPointer gdt_ptr;
GDTEntry gdt[5] = {
  //null descriptor
  { .limit_low = 0,
    .base_low = 0,
    .base_mid = 0,
    .access = 0,
    .flags_limit_high = 0,
    .base_high = 0
  },
  
  //gdt code
  { .limit_low = 0xFFFF,
    .base_low = 0x0000,
    .base_mid = 0x0000,
    .access = 0b10011010,
    .flags_limit_high = 0b11001111,
    .base_high = 0x0000
  },

  //gdt data
  { .limit_low = 0xFFFF,
    .base_low = 0x0000,
    .base_mid = 0x0000,
    .access = 0b10010010,
    .flags_limit_high = 0b11001111,
    .base_high = 0x0000
  },
  
  //gdt user code
  { .limit_low = 0xFFFF,
    .base_low = 0x0000,
    .base_mid = 0x0000,
    .access = 0b11111010,
    .flags_limit_high = 0b11001111,
    .base_high = 0x0000
  },

  //gdt user data
  { .limit_low = 0xFFFF,
    .base_low = 0x0000,
    .base_mid = 0x0000,
    .access = 0b11110010,
    .flags_limit_high = 0b11001111,
    .base_high = 0x0000
  },
};

void initialize_gdt_table(){
	gdt_ptr.limit = sizeof(gdt);
	gdt_ptr.base = gdt;
	// We have to load the gdt ptr,
	// perform a far jump to load the proper cs
	// and load all the other segment registers.
	asm("lgdt %0\n"
		"ljmpl $0x08, $gdt_fin\n"
		"gdt_fin:\n"
		"mov %%ax, %%ds\n"
		"mov %%ax, %%es\n"
		"mov %%ax, %%gs\n"
		"mov %%ax, %%fs\n"
		"mov %%ax, %%ss\n"
		:: "m"(gdt_ptr), "a"(0x10)
		: "memory");
}

static void asm_dummy(){
	asm("get_eflags:\n"
		"	pushf\n"
		"	pop %%eax\n"
		"	ret\n" ::);
}

void sti(){
	asm("sti");
}

void cli(){
	asm("cli");
}

NoInterrupts::NoInterrupts(){
	reenable = get_eflags() & 0x200;
	cli();
}

NoInterrupts::~NoInterrupts(){
	if(reenable)
		sti();
}

void panic(char *msg){
	NoInterrupts ni;
	com1().write_string(msg);
	asm("hlt");
}
