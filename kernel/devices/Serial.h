#pragma once
#include <stdint.h>
#include <stddef.h>

#define COM1 0x3F8  

class Serial{

public:
	Serial(int port);
	int putchar(const char);
	int write(const char*, size_t);
	int write_string(const char*);
private:
	int m_port { 0 };

	bool is_empty();
};

Serial& com1();

Serial& operator<<(Serial& serial, char*);
Serial& operator<<(Serial& serial, int);
Serial& operator<<(Serial& serial, void*);
