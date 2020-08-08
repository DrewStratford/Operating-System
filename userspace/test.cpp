#include <System.h>
#include <string.h>
#include <FileStream.h>
#include <Time.h>

void prompt(FileStream& stream){
	Time time = from_timecode(get_time());
	stream << time.hour << ":" << time.minute << ":" << time.second << " $ ";
}

void strip_newline(char* buffer){
	for(char* c = buffer; *c != '\0'; c++){
		if(*c == '\n')
			*c = '\0';
	}
}


int main(void){
	debug("hello from userspace\n");	
	int stdfd = 0;
	FileStream stream(stdfd);
	char buf[61];
	int c = 0;

	prompt(stream);
	while(c = read(stdfd, buf, 0, 60)){
		buf[c] = '\0';
		strip_newline(buf);
		int fds[] = { stdfd };
		int r = create_thread(buf, 1, fds);
		stream << "r=" << r << "\n";
		prompt(stream);
	}
	debug("exiting test\n");
	exit();
	return 0;
}
