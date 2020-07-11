#include <System.h>
#include <string.h>
#include <FileStream.h>

int main(void){
	debug("hello from userspace\n");	
	int stdfd = 0;
	FileStream stream(stdfd);
	char buf[61];
	int c = 0;

	while(c = read(stdfd, buf, 0, 60)){
		buf[c] = '\0';
		stream << "-->: " << buf;
	}
	debug("exiting test\n");
	exit();
	return 0;
}
