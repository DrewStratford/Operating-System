#pragma once
#include <stddef.h>

class OutStream{
public:
	virtual int putchar(const char) = 0;
	virtual int write(const char*, size_t);
	int write_string(const char*);
};

OutStream& operator<<(OutStream& stream, char*);
OutStream& operator<<(OutStream& stream, int);
OutStream& operator<<(OutStream& stream, void*);
OutStream& operator<<(OutStream& stream, bool);
