#pragma once
#include <stddef.h>
#include <devices/CPU.h>

typedef void (*InterruptCallback)(Registers&);

void initialize_interrupts();
void register_interrupt_callback(InterruptCallback callback, size_t no);
