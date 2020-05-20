#include <Stream.h>
#include <string.h>
#include <stdint.h>

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


OutStream& operator<<(OutStream& stream, char* str){
	stream.write_string(str);
	return stream;
}

static const char *digits = "0123456789abcdef";
void write_int(OutStream& stream, int32_t i, int base){
	char buffer[12] = { 0 };
	char *idx = &buffer[10];
	uint32_t integer = i;

	switch(base){
		case 2:
			stream.write_string("0b");
			break;
		case 16:
			stream.write_string("0x");
			break;
	}

	if(base == 10 && i < 0){
		integer = -i;
		stream.write_string("-");
	}

	do {
		int d = integer % base;
		integer /= base;
		*idx = digits[d];
		idx--;
	} while (integer > 0);
	idx++;

	stream.write_string(idx);
}

OutStream& operator<<(OutStream& stream, int i){
	write_int(stream, i, 10);
	return stream;
}

OutStream& operator<<(OutStream& stream, void* i){
	write_int(stream, (uint32_t)i, 16);
	return stream;
}

OutStream& operator<<(OutStream& stream, bool b){
	const char *symbol = b ? "true" : "false";
	stream.write_string(symbol);
	return stream;
}
