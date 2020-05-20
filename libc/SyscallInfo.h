#pragma once

#include <stdint.h>

// This file provides both: The syscall number, and argument structure for 
// a given syscall.

#define SC_debug 0
#define SC_create_thread 1
#define SC_exit_thread 2
#define SC_open_file 3
#define SC_close_file 4
#define SC_read	5
