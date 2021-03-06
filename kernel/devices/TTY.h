#pragma once

#include <stdint.h>
#include <filesystem/FileSystem.h>
#include <Stream.h>
#include <InterruptQueue.h>

class VGATerminal : public File {

public:
	int putchar(const char);
	void scroll_up();

	void clear_line(int);
	void clear();

	virtual size_t size() override;
	virtual size_t write(char*, size_t offset, size_t amount) override;
	virtual size_t read(char*, size_t offset, size_t amount) override;

	void emit(char);
	void input(char*, size_t amount);

	void backspace();
	bool can_backspace();

	void clear_screen();

	InterruptQueue<uint8_t> interrupt_channel;

private:
	int x { 0 };
	int y { 0 };

	// In the future we should ensure this belongs to a proper memory
	// region.
	uint16_t* screen { (uint16_t*) 0xb8000 };
	static constexpr int width { 80 };
	static constexpr int height { 25 };
	uint16_t screen_buffer[width*height];

	int line_count { 0 };

	uint8_t foreground { 0 };
	uint8_t background { 0xF };

	void draw_at(int x, int y, char);
	void draw_at(size_t index, uint16_t);
	void update_cursor();

	bool m_escape { false };

	Vector<char> m_data;
	Lock m_lock;
	ConditionVar m_cvar;
};
