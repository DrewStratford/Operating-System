#include <System.h>
#include <string.h>

int main(void){
	debug("hello from userspace\n");	
	int fd = open("console");
	char buf[61];
	int c = 0;

	while(c = read(fd, buf, 0, 60)){
		buf[c] = '\0';
		debug(buf);
		write(fd, (void*)"-->: ", 0, strlen("-->: "));
		write(fd, buf, 0, strlen(buf));
	}
	debug("exiting test\n");
	exit();
	return 0;
}
