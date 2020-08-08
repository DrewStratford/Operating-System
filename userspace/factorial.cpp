#include <System.h>
#include <string.h>
#include <FileStream.h>

int factorial(int i){
	if(i <= 0)
		return 1;
	return i * factorial(i-1);
}

int main(void){
	int stdfd = 0;
	FileStream stream(stdfd);
	
	stream << "factorial(5)=" << factorial(5) << "\n";
	exit();
	return 0;
}
