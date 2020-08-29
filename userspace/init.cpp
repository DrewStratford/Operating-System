#include <System.h>
#include <string.h>

char* init_message =
"welcome to OS (name change pending)\n\
Running init ...\n";

int main(void){
	int console = open("console");
	write(console, init_message, 0, strlen(init_message));
	int fds[] = { console };
	create_thread("vfs/test.prog", 1, fds );
	return 0;
}

