#include <string.h>

#include <devices/Serial.h>
#include <devices/IO.h>

Serial p_com1(COM1);
Serial::Serial(int port){
	m_port = port;
	IO::out8(m_port + 1, 0x00);    // Disable all interrupts
	IO::out8(m_port + 3, 0x80);    // Enable DLAB (set baud rate divisor)
	IO::out8(m_port + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
	IO::out8(m_port + 1, 0x00);    //                  (hi byte)
	IO::out8(m_port + 3, 0x03);    // 8 bits, no parity, one stop bit
	IO::out8(m_port + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
	IO::out8(m_port + 4, 0x0B); 
}

Serial& com1(){
	return p_com1;
}

bool Serial::is_empty(){
	return IO::in8(m_port + 5) & 0x20;
}

int Serial::putchar(char c) {
   while (is_empty() == 0);
   IO::out8(m_port, c);
   return 0;
}

int Serial::write(char* cs, size_t size){
	for(size_t i = 0; i < size; i++){
		putchar(cs[i]);
	}

	return size;
}

int Serial::write_string(char* string){
	write(string, strlen(string));
}
