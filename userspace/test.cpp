#include <System.h>
#include <string.h>

int main(void){
	debug("hello from userspace");	
	int fd = open("console");
	char buf[11];
	int c = 0;

	while(c = read(fd, buf, 0, 10)){
		buf[c] = '\0';
		debug(buf);
		write(fd, (void*)"-->: ", 0, strlen("-->: "));
		write(fd, buf, 0, strlen(buf));
	}
	exit();
	return 0;
}
