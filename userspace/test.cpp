#include <System.h>
#include <string.h>
#include <FileStream.h>
#include <Time.h>

void prompt(FileStream& stream){
	Time time = from_timecode(get_time());
	stream << time.hour << ":" << time.minute << ":" << time.second << " $ ";
}

int main(void){
	debug("hello from userspace\n");	
	int stdfd = 0;
	FileStream stream(stdfd);
	char buf[61];
	int c = 0;

	stream << "timestamp " << get_time() << "\n";

	prompt(stream);
	while(c = read(stdfd, buf, 0, 60)){
		buf[c] = '\0';
		stream << "-->: " << buf;
		prompt(stream);
	}
	debug("exiting test\n");
	exit();
	return 0;
}
