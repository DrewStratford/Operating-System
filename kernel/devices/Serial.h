#pragma once
#include <stdint.h>
#include <stddef.h>

#define COM1 0x3F8  

class Serial{

public:
	Serial(int port);
	int putchar(char);
	int write(char*, size_t);
	int write_string(char*);
private:
	int m_port { 0 };

	bool is_empty();
};

Serial& com1();
