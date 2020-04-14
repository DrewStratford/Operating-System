#include <filesystem/FileSystem.h>

#include <string.h>

DirectoryEntry::DirectoryEntry(char* name, Inode* inode)
	: m_name(name), m_inode(inode){
}

bool DirectoryEntry::is_named(char* name){
	return strcmp(m_name, name) == 0;
}

size_t File::size(){
	return m_data.size();
}

size_t File::read(char* buffer, size_t offset, size_t amount){
	if(offset > m_data.size())
		return 0;
	
	size_t ret = 0;
	for(size_t i = offset; i < amount && i < size(); i++, ret++)
		buffer[ret] = m_data[i];

	return ret;
}

size_t File::write(char* buffer, size_t offset, size_t amount){
	if(offset > m_data.size())
		return 0;
	
	size_t ret = 0;
	for(size_t i = offset; i < amount; i++, ret++)
		m_data.insert(i, buffer[ret]);

	return ret;
}

Directory::Directory(){
	add_entry(".", this);
}

bool Directory::add_entry(char* name, Inode* inode){
	m_entries.insert_end(DirectoryEntry(name, inode));
	return true;
}

DirectoryEntry* Directory::lookup_entry(char* name){
	for(size_t i = 0; i < m_entries.size(); i++){
		DirectoryEntry* entry = &m_entries[i];
		if(entry->is_named(name))
			return entry;
	}
	return nullptr;
}

bool Directory::create_file(char* name){
	File* file = new File();
	add_entry(name, file);
	return true;
}

bool Directory::create_subdirectory(char* name){
	Directory* directory = new Directory();
	directory->add_entry("..", this);
	add_entry(name, directory);
	return true;
}


// This code is used to load the initramfs into
// the virtual file system.
#define FILE_NAME_LIMIT 50
struct HeaderEntry{
  char name[FILE_NAME_LIMIT+1];
  uint32_t file_size;
  uint32_t file_pointer;
}__attribute__((packed));

bool load_init_ramfs(Directory* root_dir, uint32_t* init){
	uint32_t magic = init[0];
	uint32_t file_count = init[1];
	HeaderEntry* headers = (HeaderEntry*)&init[2];

	root_dir->create_subdirectory("vfs");
	if(Directory* dir = root_dir->lookup_entry("vfs")->get_inode()->as_directory()){
		for(int i = 0; i < file_count; i++){
			File* file = new File();
			char* file_mem = (char*)init + (size_t)headers->file_pointer;
			file->write(file_mem, 0, headers->file_size);
			dir->add_entry(headers->name, file);
			headers++;
		}
		return true;
	}
	return false;
}
