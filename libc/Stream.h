#pragma once
#include <stddef.h>

class OutStream{
public:
	virtual int putchar(const char) = 0;
	virtual int write(const char*, size_t);
	int write_string(const char*);
};
