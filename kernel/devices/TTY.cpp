#include <devices/TTY.h>
#include <devices/IO.h>
#include <string.h>

static inline uint8_t vga_entry_colour(uint8_t fg, uint8_t bg) {
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}

// I'm not entirely sure how it works.
//
// I guess 0x3d4 is the vga controllers command register
// and 0x3d5 is the data register.
//
// Following Ralph Brown's port guide:
// 0x3d4 specifies the register to be set
// by 0x3d5. 0x0F is cursor low, 0x0E is cursor high.
void VGATerminal::update_cursor(){
	uint16_t pos = y * width + x;

	IO::out8(0x3D4, 0x0F);
	IO::out8(0x3D5, (uint8_t)(pos & 0xFF));
	IO::out8(0x3D4, 0x0E);
	IO::out8(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void VGATerminal::draw_at(size_t index, uint16_t out){
	uint8_t colour = vga_entry_colour(foreground, background);

	uint16_t previous = screen_buffer[index];
	if(out != previous){
		screen_buffer[index] = out;
		screen[index] = out;
	}
}

void VGATerminal::draw_at(int x, int y, char c){
	uint8_t colour = vga_entry_colour(foreground, background);
	uint16_t out = vga_entry(c, colour);

	draw_at(y * width + x, out);
}

void VGATerminal::scroll_up(){
	size_t screen_end = (height*width)-width;

	for(size_t i = 0; i < screen_end; i++)
		draw_at(i, screen_buffer[i + width]);

	y--;
	x = 0;
	clear_line(height-1);
}

void VGATerminal::clear_line(int line){
	uint8_t colour = vga_entry_colour(foreground, background);
	uint16_t out = vga_entry(' ', colour);
	for(int i = 0; i < width; i++) {
		screen[line*width + i] = out;
		screen_buffer[line*width + i] = out;
	}
}

void VGATerminal::clear(){
	x = 0;
	y = 0;

	for(int i = 0; i < height; i++)
		clear_line(i);

	update_cursor();
}

int VGATerminal::putchar(char c){
	switch(c){
		case '\n':
			x = 0;
			y++;
			break;
		case '\b':
			backspace();
			break;
		default:
			draw_at(x, y, c);
			x++;
			break;
	}

	if(x >= width)
		putchar('\n');

	if(y >= height)
		scroll_up();
	return 0;
}

size_t VGATerminal::size(){
	return 0;
}

// A write just draws to the console.
size_t VGATerminal::write(char* cs, size_t offset, size_t amount){
	ScopedLocker locker(&m_lock);
	for(int i = 0; i < amount; i++)
		putchar(cs[i]);

	return amount;
}

// reads from a blocking buffer. Data is supplied to the buffer
// by the keyboard driver.
size_t VGATerminal::read(char* cs, size_t offset, size_t amount){
	ScopedLocker locker(&m_lock);
	while(line_count < 1)
		m_cvar.wait(m_lock);
	
	size_t out = 0;
	for(; out < amount && !m_data.is_empty(); out++){
		char c = m_data.remove_front();
		if(c == '\0'){
			line_count--;
			break;
		} else if( c == '\n'){
			cs[out] = c;
			line_count--;
			out++;
			break;
		}
			
		cs[out] = c;
	}

	// If we didn't wake here any waiting threads would be starved till
	// the next write
	if(line_count > 0)
		m_cvar.wake();

	return out;
}

bool VGATerminal::can_backspace(){
	size_t end = m_data.size()-1;
	return !m_data.is_empty() && 
		m_data[end] != '\0' && 
		m_data[end] != '\n';
}

void VGATerminal::backspace(){
	if(can_backspace()){
		m_data.remove_end();
		x--;
		if(x < 0){
			x = width-1;
			y <= 0 ? 0 : y--;
		}
		draw_at(x, y, '\0');
	}
}

void VGATerminal::emit(char c){
	switch(c){
		case '\0':
			break;
		case '\n':
			putchar('\n');
			m_data.insert_end('\n');
			line_count++;
			m_cvar.wake();
			break;
		case '\b':
			putchar('\b');
			break;
		//handle ^D
		case 0x04:
			m_data.insert_end('\0');
			line_count++;
			m_cvar.wake();
			break;

		default:
			putchar(c);
			m_data.insert_end(c);
			break;
	}
	update_cursor();
}

// This provides input to the data queue. The data is also echoed to the
// screen.
void VGATerminal::input(char* cs, size_t amount){
	ScopedLocker locker(&m_lock);
	for(int i = 0; i < amount; i++)
		emit(cs[i]);

	update_cursor();
}
