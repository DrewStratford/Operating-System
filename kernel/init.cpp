#include <stdint.h>

static inline uint8_t vga_entry_color(uint8_t fg, uint8_t bg) 
{
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

extern "C" int kernel_main(){
	uint16_t *buffer = (uint16_t*)0xb8000;
	buffer[0] = vga_entry('h', vga_entry_color(8, 0));
	buffer[1] = vga_entry('i', vga_entry_color(9, 0));
	return 0;
}
