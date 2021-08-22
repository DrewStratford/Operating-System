#include <System.h>
#include <string.h>
#include <FileStream.h>

void write_out(int fd){
	FileStream stream(0);
	char buf[1025];
	int c = 0;
	int offset = 0;

	while((c = read(fd, buf, offset, 1024))){
		buf[c] = '\0';
		offset += c;
		stream << buf;
	}
}
int main(int argc, char** argv){
	FileStream stream(0);

	for(int i = 1; i < argc; i++){
		// "-" is read from stdin
		int fd = string(argv[i]) == string("-") ? 0 : open(argv[i]);
		if(fd < 0)
			return -1;

		write_out(fd);
		if(fd != 0)
			close(fd);
	}
	return 0;
}
