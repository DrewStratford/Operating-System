#include <System.h>
#include <string.h>
#include <FileStream.h>
#include <Malloc.h>

int factorial(int i){
	if(i <= 0)
		return 1;
	return i * factorial(i-1);
}

int main(int argc, char** argv){
	int stdfd = 0;
	FileStream stream(stdfd);

	stream << "argc=" << argc << "\n";
	for(int i = 0; i < argc; i++){
		stream << argv[i] << "\n";
	}
	
	stream << "factorial(5)=" << factorial(5) << "\n";
	exit(factorial(5));
	return 0;
}
