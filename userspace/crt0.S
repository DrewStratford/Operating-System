#include <SyscallInfo.h>
	.text

.globl _start
_start:
	call main
	# call exit with return code 0
	push $0x0
	mov $SC_exit_thread, %eax
	int $0x80
