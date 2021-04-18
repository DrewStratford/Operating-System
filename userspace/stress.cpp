#include <Signal.h>
#include <System.h>
#include <string.h>
#include <FileStream.h>
#include <Time.h>

void callback(void){
	debug("------>in the signal handler\n");
}

int i = 0;
void handle_sigchld(void){
	debug("stress: sigchld\n");
	i++;
	debug("bro\n");
	if(i < 10)
		debug("10\n");
	else if( i < 30)
		debug("30\n");
	else if(i < 50)
		debug("50\n");
	else if(i < 70)
		debug("70\n");
	else if(i < 90)
		debug("90\n");
	else
		debug(">90\n");
	i--;
	/*
	//if(i != 200){
	//	kill(-1, SIGCHLD);
	//}
	//debug("sigchld");
	*/
}

int main(void){
	debug("\nstress testing yepasdfasd\n");
	int stdfd = 0;
	FileStream stream(stdfd);
	char* buf = "vfs/factorial.prog";
	int c = 0;
	stream << "callback " << (void*)handle_sigchld << "\n";

	//signal(SIGINT, (uintptr_t)callback);
	signal(SIGCHLD, (uintptr_t)handle_sigchld);
	//kill(-1, SIGCHLD);


	for(int i = 0; i < 120; i++){
		int fds[] = { stdfd };
		int r = create_thread(buf, 1, fds);
		//stream << "created " << i << "\n";
		//stream << "ret: " << wait(r) << "\n";
	}

	stream << "finished stress test\n";
	debug("finished !!!\n");
	return 0;
}

