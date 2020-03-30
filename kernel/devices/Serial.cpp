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

int Serial::write(const char* cs, size_t size){
	for(size_t i = 0; i < size; i++){
		putchar(cs[i]);
	}

	return size;
}

int Serial::write_string(const char* string){
	int length = strlen(string);
	write(string, length);
	return length;
}

Serial& operator<<(Serial& serial, char* str){
	serial.write_string(str);
	return serial;
}

static const char *digits = "0123456789abcdef";
void write_int(Serial& serial, int32_t i, int base){
	char buffer[12] = { 0 };
	char *idx = &buffer[10];
	uint32_t integer = i;

	switch(base){
		case 2:
			serial.write_string("0b");
			break;
		case 16:
			serial.write_string("0x");
			break;
	}

	if(base == 10 && i < 0){
		integer = -i;
		serial.write_string("-");
	}

	do {
		int d = integer % base;
		integer /= base;
		*idx = digits[d];
		idx--;
	} while (integer > 0);
	idx++;

	serial.write_string(idx);
}

Serial& operator<<(Serial& serial, int i){
	write_int(serial, i, 10);
	return serial;
}

Serial& operator<<(Serial& serial, void* i){
	write_int(serial, (uint32_t)i, 16);
	return serial;
}

Serial& operator<<(Serial& serial, bool b){
	const char *symbol = b ? "true" : "false";
	serial.write_string(symbol);
	return serial;
}
