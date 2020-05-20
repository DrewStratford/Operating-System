#pragma once
#include <stdint.h>
#include <stddef.h>

#include <Stream.h>

#define COM1 0x3F8  

class Serial : public OutStream {

public:
	Serial(int port);
	int putchar(const char);
private:
	int m_port { 0 };

	bool is_empty();
};

Serial& com1();
