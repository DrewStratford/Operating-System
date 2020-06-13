#pragma once

#include <stdint.h>
#include <filesystem/FileSystem.h>
#include <devices/TTY.h>

class Keyboard{
public:

	void set_shift(bool);
	char* handle_code(uint8_t);
private:
	bool m_is_shift { false };
};

void initialize_keyboard(VGATerminal*);
File& keyboard_file();
