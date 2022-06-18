#include <System.h>
#include <string.h>
#include <FileStream.h>

void list_directory(FileStream& stream, string path){
	int dir = opendir(path.m_str);
	for(auto i = 0; i < 10; i++){
		UserDirectoryEntry de;
		if(readdir(dir, (void*)&de, i) < 0 )
			break;
		stream << de.name << "\n";
	}
	close(dir);
}
int main(int argc, char** argv){
	debug("ls hello");
	FileStream stream(0);
	debug("ls hello2");

	if(argc == 2){
		list_directory(stream, argv[1]);
		return 0;
	}

	for(auto i = 1; i < argc; i++){
		stream << argv[i] << "\n";
		list_directory(stream, argv[i]);
	}
	return 0;
}
