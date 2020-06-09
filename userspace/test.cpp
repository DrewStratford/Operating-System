#include <System.h>

int main(void){
	debug("hello from userspace");	
	int fd = open("keyboard");
	char buf[11];
	int c = 0;
	while(c = read(fd, buf, 0, 10)){
		buf[c] = '\0';
		debug(buf);
	}
	exit();
	return 0;
}
