#pragma once

#include <stdint.h>
#include <filesystem/FileSystem.h>

class Keyboard{
public:

	void set_shift(bool);
	char* handle_code(uint8_t);
private:
	bool m_is_shift { false };
};

void initialize_keyboard();
File& keyboard_file();
