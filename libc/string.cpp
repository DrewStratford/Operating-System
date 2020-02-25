#include <string.h>
#include <stdint.h>

void* memcpy(void* dest, const void* src, size_t n){
	const uint8_t* in = (uint8_t*)src;
	uint8_t* out = (uint8_t*)dest;
	for(size_t i = 0; i < n; i++)
		out[i] = in[i];
	return dest;
}

int memcmp(const void *s1, const void * s2, size_t n){
	const uint8_t* a = (uint8_t*)s1;
	const uint8_t* b = (uint8_t*)s2;
	for(size_t i = 0; i < n; i++){
		if(a[i] < b[i]) return -1;
		if(a[i] > b[i]) return 1;
	}
	return 0;
}

void* memchr(void *s, int c, size_t n){
	uint8_t *a = (uint8_t*)s;
	for(size_t i = 0; i < n; i++){
		if(a[i] == (uint8_t)c)
			return &a[i];
	}
	return NULL;
}

void* memset(void *s, int c, size_t n){
	uint8_t *ptr = (uint8_t*) s;
	for(size_t i = 0; i < n; i++){
		ptr[i] = c;
	}
	return s;
}

size_t strlen(const char *in){
	size_t out = 0;
	while(*in != '\0'){
		in++;
		out++;
	}
	return out;
}

int strcmp(const void *s1, const void * s2){
	const char* a = (const char*)s1;
	const char* b = (const char*)s2;

	int len_a = strlen(a);
	int len_b = strlen(b);

	if(len_a < len_b) return -1;
	if(len_a > len_b) return 1;

	for(int i = 0; i < len_a; i++){
		if(a[i] < b[i]) return -1;
		if(a[i] > b[i]) return 1;
	}
	return 0;
}
