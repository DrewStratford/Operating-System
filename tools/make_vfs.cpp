#include <cstring>
#include <string>
#include <vector>

#include <sys/stat.h>

#include <fstream>
#include <iostream>

long getFileSize(char* filename){
  struct stat stat_buf;
  int err_code = stat(filename, &stat_buf);
  return err_code == 0 ? stat_buf.st_size : -1;
}

class FileInfo{
public:
  long size;
  char* file_name;

  FileInfo(char* file_name);
  void WriteOut(std::ofstream& file);

};

FileInfo::FileInfo(char* file_name){
  this->file_name = file_name;
  this->size = getFileSize(file_name);
}

void FileInfo::WriteOut(std::ofstream& out_file){
  std::ifstream in_file;
  in_file.open(this->file_name, std::ios::binary);
  char data = in_file.get();
  while(in_file.good()){
    out_file << data;
    data = in_file.get();
  }
  in_file.close();
}

const long FILE_NAME_LIMIT = 50;
struct HeaderEntry{
  char name[FILE_NAME_LIMIT+1];
  uint32_t file_size;
  uint32_t file_pointer;
}__attribute__((packed));

class Header{
public:
  uint32_t magic;
  uint32_t no_entries;
  int header_size;
  std::vector<HeaderEntry*> entries;
  Header(std::vector<FileInfo>& files);
  void WriteOut(std::ofstream& file);

};


Header::Header(std::vector<FileInfo>& files){
  this->magic = 0x07654321;
  this->no_entries = files.size();
  this->header_size = sizeof(HeaderEntry) * this->no_entries;
  this->entries = std::vector<HeaderEntry*>();

  uint32_t offset = header_size+sizeof(no_entries)+sizeof(magic);
  
  for(auto t : files){
    struct HeaderEntry *new_entry = new HeaderEntry();
    std::memcpy(new_entry->name, t.file_name, std::strlen(t.file_name));
    new_entry->name[FILE_NAME_LIMIT] = '\0';
    new_entry->file_size = t.size;
    new_entry->file_pointer = offset;
    entries.push_back(new_entry);

    offset+=t.size;
  }
}

void Header::WriteOut(std::ofstream& file){
  file.write((char*)&this->magic, sizeof(uint32_t));
  file.write((char*)&this->no_entries, sizeof(uint32_t));
  for(auto entry : this->entries){
    file.write((char*)entry, sizeof(struct HeaderEntry));
  }
}

int main(int argc, char *argv[]){
  auto files = std::vector<FileInfo>();
  for(int i = 1; i < argc; i++){
    files.push_back(argv[i]);
  }

  long total_length = 0;
  for(auto t : files){
    total_length+= t.size;
    //std::cout<<t.file_name << ", " << t.size << std::endl;
  }

  //std::cout << "total length " << total_length << std::endl;
  Header headers = Header(files);
  for(auto hep : headers.entries){
    std::cout << hep->name << ", offset: " << std::hex << hep->file_pointer
	      << ", size : " << hep->file_size << std::endl;
  }
  std::ofstream out_file;
  out_file.open("myvfs.vfs", std::ios::binary);
  headers.WriteOut(out_file);
  for(auto f_info : files){
    f_info.WriteOut(out_file);
  }
  out_file.close();
  return 0;
}
