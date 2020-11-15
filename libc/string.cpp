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

string::string(){
	m_size = 0;
	m_str = nullptr;
}

string::~string(){
	if(m_str)
		delete[] m_str;
}

string::string(const char* str){
	m_size = strlen(str) + 1;
	m_str = new char[m_size];
	memcpy(m_str, (const void*)str, m_size);
	m_str[m_size - 1] = '\0';
}

string::string(const char* string, int size){
	m_size = size + 1;
	m_str = new char[m_size];
	memcpy(m_str, string, m_size);
	m_str[m_size - 1] = '\0';
}

string::string(const string& str){
	m_size = str.length() + 1;
	m_str = new char[m_size];
	memcpy(m_str, str.m_str, m_size);
	m_str[m_size - 1] = '\0';
}

string& string::operator=(const string& str){
	m_size = str.length() + 1;
	m_str = new char[m_size];
	memcpy(m_str, str.m_str, m_size);
	m_str[m_size - 1] = '\0';
	return *this;
}

string& string::operator=(const char* str){
	m_size = strlen(str) + 1;
	m_str = new char[m_size];
	memcpy(m_str, (const void*)str, m_size);
	m_str[m_size - 1] = '\0';
	return *this;
}

string string::append(const string& str) const{
	auto new_size = this->length() + str.length() + 1;
	char* new_c_str = new char[new_size];
	memcpy(new_c_str, (const void*)this->m_str, this->length());
	memcpy(new_c_str + this->length(), str.m_str, str.length());
	new_c_str[new_size-1] = '\0';

	return string(new_c_str);
}

string string::substring(int start, int size) const{
	if(start < 0)
		return string(*this);
	if(length() < start)
		return string("");

	int to_end = length() - start;
	size = to_end < size ? to_end : size;
	return string(&m_str[start], size);
}

string string::substring(int start) const{
	int to_end = length() - start;
	return substring(start, to_end);
}

char string::at(size_t i) const{
	return m_str[i];
}

char& string::operator[](int index){
	return m_str[index];
}

bool string::operator==(const string& r) const{
	if(length() == r.length())
		return memcmp(m_str, r.m_str, length()) == 0;
	return false;
}

int string::index_of(char c) const{
	for(int i = 0; i < length(); i++){
		if(at(i) == c)
			return i;
	}
	return -1;
}

size_t string::length() const{
	return m_size - 1;
}

OutStream& operator<<(OutStream& stream, string& str){
	if(str.m_str == nullptr){
		stream << "nullptr";
	} else {
		stream << str.m_str;
	}
	return stream;
}
