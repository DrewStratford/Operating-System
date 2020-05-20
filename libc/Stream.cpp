#include <Stream.h>
#include <string.h>

int OutStream::write(const char* str, size_t size){
	for(size_t i = 0; i < size; i++)
		putchar(str[i]);

	return size;
}

int OutStream::write_string(const char* string){
	int length = strlen(string);
	write(string, length);
	return length;
}


