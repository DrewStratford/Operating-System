#include <Signal.h>
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

void callback(void){
	debug("------>in the signal handler\n");
}

void handle_sigchld(void){
	debug("sigchld\n");
}

int main(void){
	debug("hello from userspace\n");
	int stdfd = 0;
	FileStream stream(stdfd);
	char buf[61];
	volatile int32_t c = 0;

	signal(SIGINT, (uintptr_t)callback);
	signal(SIGCHLD, (uintptr_t)handle_sigchld);
	kill(-1, SIGINT);
	kill(-1, SIGCHLD);

	prompt(stream);
	while(c = read(stdfd, buf, 0, 60)){
		if(c < 0) continue;
		buf[c] = '\0';
		strip_newline(buf);
		int fds[] = { stdfd };
		int r = create_thread(buf, 1, fds);
		//stream << "ret: " << wait(r) << "\n";

		prompt(stream);
	}
	debug("exiting test\n");
	return 0;
}
