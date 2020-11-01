#include <SyscallInfo.h>
	.text

.globl _start
_start:
	# we assume that start and end were pushed by the kernel
	call initialize_heap
	add $0x8, %esp
	call main
	# call exit with return code 0
	push $0x0
	mov $SC_exit_thread, %eax
	int $0x80
