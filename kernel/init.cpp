#include <icxxabi.h>
#include <Thread.h>
#include <devices/IO.h>
#include <devices/Serial.h>
#include <devices/CPU.h>

static inline uint8_t vga_entry_color(uint8_t fg, uint8_t bg) 
{
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}


void foo1();
void foo2();

uint32_t stack1[1000];
uint32_t stack2[1000];
Thread kernel = Thread(0,0);
Thread a = Thread((uint32_t)stack1, (uintptr_t)foo1);
Thread b = Thread((uint32_t)stack2, (uintptr_t)foo2);

void foo1(){
	while(true){
		Serial serial(COM1);
		serial.write("1\n", 2);
		Thread::switch_thread(a, b);
	}
}

void foo2(){
	while(true){
		Serial serial(COM1);
		serial.write("2\n", 2);
		Thread::switch_thread(b, kernel);
	}
}

extern "C" int kernel_main(){
	Serial serial(COM1);
	serial.write("hello\n", 6);

	initialize_gdt_table();
	serial.write("done", 4);

	kernel = Thread(0,0);
	a = Thread((uint32_t)&stack1[999], (uintptr_t)foo1);
	b = Thread((uint32_t)&stack2[999], (uintptr_t)foo2);

	while(true){
	Thread::switch_thread(kernel, a);
	serial.write("back in kernel\n", 15); 
	}

	return 0;
}
