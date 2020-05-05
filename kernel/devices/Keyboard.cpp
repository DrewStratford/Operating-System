#include <devices/Keyboard.h>

#include <devices/IO.h>
#include <devices/Interrupts.h>

#include <devices/Serial.h>

#include <string.h>


Keyboard* keyboard = nullptr;
DeviceFile* key_stream = nullptr;

char* key_map[] = {
	"\0",
	"\0",
	"1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
	"-", "=", "\b", "\t",
	"q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]",
	"\n", "\0",
	"a", "s", "d", "f", "g", "h", "j", "k", "l",
	";", "\"", "`", "\0", "\\",
	"z", "x", "c", "v", "b", "n", "m", ",", ".", "/",
	"\0", "\0", "\0", " ", "\0", 
	"\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0", 
	"\0", "\0", 
	"\0", "\0", "\0", "\0",
	"\0", "\0", "\0", "\0",
	"\0", "\0", "\0", "\0",
	"\0", "\0", "\0", "\0", "\0",
	"\0",
	"\0",
	"\0",
	"\0", "\0"
};

char* key_map_shifted[] = {
	"\0",
	"\0",
	"!", "@", "#", "$", "%", "^", "&", "*", "(", ")",
	"_", "+", "\b", "\t",
	"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}",
	"\n", "\0",
	"A", "S", "D", "F", "G", "H", "J", "K", "L",
	":", "\"", "~", "\0", "|",
	"Z", "X", "C", "V", "B", "N", "M", "<", ">", "?",
	"\0", "\0", "\0", " ", "\0", 
	"\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0", 
	"\0", "\0", 
	"\0", "\0", "\0", "\0",
	"\0", "\0", "\0", "\0",
	"\0", "\0", "\0", "\0",
	"\0", "\0", "\0", "\0", "\0",
	"\0",
	"\0",
	"\0",
	"\0", "\0"
};

void Keyboard::set_shift(bool b){
	m_is_shift = b;
}

char* Keyboard::handle_code(uint8_t key){
	char* key_str = "\0";
	if(m_is_shift){
		if(key >= sizeof(key_map_shifted) / sizeof(char*))
			return "\0";
		key_str = key_map_shifted[key];
	} else {
		if(key >= sizeof(key_map) / sizeof(char*))
			return "\0";
		key_str = key_map[key];
	}
	return key_str;
}

void keyboard_irq(Registers& rs){
	uint8_t key = IO::in8(0x60);

	if(key == 42 || key == 54)
		keyboard->set_shift(true);

	if(key == 182 || key == 170)
		keyboard->set_shift(false);

	char* key_code = keyboard->handle_code(key);
	keyboard_file().write(key_code, 0, strlen(key_code));
}

void initialize_keyboard(){
	keyboard = new Keyboard();
	key_stream = new DeviceFile();
	register_interrupt_callback(keyboard_irq, 0x21);
}

File& keyboard_file(){
	return *key_stream;
}
