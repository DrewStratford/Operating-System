#pragma once
#include <Stream.h>
#include <System.h>


class FileStream : public OutStream{
public:
	FileStream(int i);
	virtual int putchar(const char);
	virtual int write(const char*, size_t);

private:
	int inode { -1 };
	int offset { 0 };
};
