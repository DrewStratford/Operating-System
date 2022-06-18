#pragma once

#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include "SyscallInfo.h"

int exit(int);

int debug(char*);

int open(char*);
int opendir(char*);
int close(int fd);
int read(int fd, void* buffer, size_t offset, size_t count);
int readdir(int fd, void* buffer, size_t offset);
int write(int fd, void* buffer, size_t offset, size_t count);
int create_thread(char* cs, size_t inode_count, int* inodes);
int create_thread(char* cs);
int32_t get_time();
int wait(int thread_id);
int signal(int signal, uintptr_t handler);
int kill(int tid, int signal);

// This is the directory entry class that we provide to the userspace.
class UserDirectoryEntry{
	public:
	char name[256];
	enum Type { F, D} type { F };
	size_t size { 0};

	UserDirectoryEntry(){
		name[0] = '\0';
	}
	UserDirectoryEntry(char* entry_name, Type type, size_t size) : type(type), size(size){
		memcpy(this->name, entry_name, 255);
		this->name[255] = '\0';
	}

};
