#include <FileStream.h>

FileStream::FileStream(int i) : inode(i){
}

int FileStream::write(const char* c, size_t s){
	int ret = ::write(inode, (void*)c, offset, s);
	offset+= ret;
	return ret;
}

int FileStream::putchar(const char c){
	return this->write(&c, 1);
}
