#pragma once

#include <sys/cdefs.h>
#include <stddef.h>

#include <Stream.h>

int memcmp(const void*, const void*, size_t);
void* memcpy(void* __restrict, const void* __restrict, size_t);
//void* memmove(void*, const void*, size_t);
void* memset(void*, int, size_t);

size_t strlen(const char*);
int strcmp(const void *s1, const void *s2);

class string{
private:
	// Note: we store the string in null terminated form.
	// m_size also counts the null byte.
	size_t m_size { 0 };

	string(const char*, int);

public:
	char* m_str { nullptr };
	string(const char* str);
	string(const string&);
	~string();

	string append(const string&) const;
	string substring(int start) const;
	string substring(int start, int end) const;

	bool operator==(const string& r);
	char& operator[](int);
	char at(size_t i);
	int index_of(char c);

	size_t length() const;

	string& operator=(const char*);

};

OutStream& operator<<(OutStream&, string&);
