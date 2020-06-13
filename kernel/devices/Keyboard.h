#pragma once

#include <stdint.h>
#include <filesystem/FileSystem.h>
#include <devices/TTY.h>

class Keyboard{
public:

	void set_shift(bool);
	void set_ctrl(bool);
	char* handle_code(uint8_t);
private:
	bool m_is_shift { false };
	bool m_is_ctrl { false };
};

void initialize_keyboard(VGATerminal*);
File& keyboard_file();
