#pragma once

#include <stdint.h>
#include <stddef.h>
#include <SyscallInfo.h>

int exit(int);

int debug(char*);

int open(char*);
int close(int fd);
int read(int fd, void* buffer, size_t offset, size_t count);
int write(int fd, void* buffer, size_t offset, size_t count);
int create_thread(char* cs, size_t inode_count, int* inodes);
int create_thread(char* cs);
int32_t get_time();
int wait(int thread_id);
int signal(int signal, uintptr_t handler);
int kill(int tid, int signal);
