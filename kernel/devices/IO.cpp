#include <devices/IO.h>

namespace IO{
	void out8(uint16_t port, uint8_t byte){
		asm volatile("outb %0, %1" :: "a"(byte), "Nd"(port)); 
	}

	void out16(uint16_t port, uint16_t byte){
		asm volatile("outw %0, %1" :: "a"(byte), "Nd"(port)); 
	}

	void out32(uint16_t port, uint32_t byte){
		asm volatile("outl %0, %1" :: "a"(byte), "Nd"(port)); 
	}

	uint8_t in8(uint16_t port){
		uint8_t byte = 0;
		asm volatile("inb %1, %0" : "=a"(byte) : "Nd"(port)); 
		return byte;
	}

	uint16_t in16(uint16_t port){
		uint16_t byte = 0;
		asm volatile("inw %1, %0" : "=a"(byte) : "Nd"(port)); 
		return byte;
	}

	uint32_t in32(uint16_t port){
		uint32_t byte = 0;
		asm volatile("inl %1, %0" : "=a"(byte) : "Nd"(port)); 
		return byte;
	}
};
