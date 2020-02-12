#include <icxxabi.h>
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

extern "C" int kernel_main(){
	Serial serial(COM1);
	serial.write("hello\n", 6);

	initialize_gdt_table();
	serial.write("done", 4);

	return 0;
}
