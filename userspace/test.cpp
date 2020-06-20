#include <System.h>
#include <string.h>

int main(void){
	debug("hello from userspace\n");	
	int stdfd = 0;
	char buf[61];
	int c = 0;

	while(c = read(stdfd, buf, 0, 60)){
		buf[c] = '\0';
		write(stdfd, (void*)"-->: ", 0, strlen("-->: "));
		write(stdfd, buf, 0, strlen(buf));
	}
	debug("exiting test\n");
	exit();
	return 0;
}
