#include <devices/TTY.h>
#include <string.h>

static inline uint8_t vga_entry_colour(uint8_t fg, uint8_t bg) {
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}

void VGATerminal::draw_at(int x, int y, char c){
	uint8_t colour = vga_entry_colour(foreground, background);
	uint16_t out = vga_entry(c, colour);

	screen[y * width + x] = out;
}

void VGATerminal::scroll_up(){
	for(int i = 0; i < height-1; i++)
		memcpy(&screen[i*width], &screen[(i+1)*width], width*2);

	y--;
	x = 0;
	clear_line(height-1);
}

void VGATerminal::clear_line(int line){
	memset(&screen[line * width], 0, width * sizeof(uint16_t));
}

void VGATerminal::clear(){
	x = 0;
	y = 0;

	for(int i = 0; i < height; i++)
		clear_line(i);
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
	for(int i = 0; i < amount; i++)
		putchar(cs[i]);

	return amount;
}

// reads from a blocking buffer. Data is supplied to the buffer
// by the keyboard driver.
size_t VGATerminal::read(char* cs, size_t offset, size_t amount){
	while(line_count < 1)
		m_cvar.wait();
	
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
}

// This provides input to the data queue. The data is also echoed to the
// screen.
void VGATerminal::input(char* cs, size_t amount){
	for(int i = 0; i < amount; i++)
		emit(cs[i]);
}