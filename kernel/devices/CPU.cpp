#include <devices/CPU.h>
#include <devices/Serial.h>


TSS tss = {
   .link = 0, .r1 = 0,
  
   .esp0 = 0,   // specifies the kernel stack on system calls
   .ss0 = 0x10, // This sets the data segment on system calls
   .r2 = 0,

   .esp1 = 0, .ss1 = 0, .r3 = 0,

   .esp2 = 0, .ss2 = 0, .r4 = 0,

   .cr3 = 0, .eip = 0, .eflags = 0, .eax = 0, .ecx = 0, .edx = 0,
   .ebx = 0, .esp = 0, .ebp = 0, .esi = 0, .edi = 0,

   .es = 0x13,  .cs = 0x0b,  .ss = 0x13,  .ds = 0x13,
    .fs = 0x13,  .gs = 0x13,

   .ldtr = 0,
   .IOPB = 0
};

GDTPointer gdt_ptr;
GDTEntry gdt[6] = {
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

  //tss
  { .limit_low = sizeof(tss),
    .base_low = 0,
    .base_mid = 0,
    .access = 0x89,
    .flags_limit_high = 0x40,
    .base_high = 0
  }
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

	// set up the tss entry's address
	uint32_t tss_addr = (uint32_t)&tss;
	gdt[5].base_low = (uint16_t)tss_addr;
	gdt[5].base_mid = tss_addr >> 16;
	gdt[5].base_high = tss_addr >> 24;

	asm("mov $0x2B, %%ax\n"
		"ltr %%ax\n"
		::: "memory");
}

TSS& get_tss(){
	return tss;
}

IDTEntry::IDTEntry(){ }

IDTEntry::IDTEntry(uint32_t base, uint8_t type){
	code_sel = 0x08;	// we always run interrupts as kernel
	// The 0x60 is used to set this decriptors privelege lvl to 3
	this->type = type | 0x60;
	offset_1 = (base & 0xFFFF);
	offset_2 = (base >> 16) & 0xFFFF;
	zero = 0;
}

IDTR::IDTR(uint16_t limit, uint32_t base){
	this->limit = limit;
	this->base = base;
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
