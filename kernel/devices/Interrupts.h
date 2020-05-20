#pragma once
#include <stddef.h>
#include <devices/CPU.h>

#include <SyscallInfo.h>

typedef void (*InterruptCallback)(Registers&);
typedef int32_t (*SystemCall)(Registers&);

void initialize_interrupts();
void register_interrupt_callback(InterruptCallback callback, size_t no);
void register_system_call(SystemCall syscall, size_t no);
