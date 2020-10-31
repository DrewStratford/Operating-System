#include <filesystem/FileSystem.h>

#include <devices/CPU.h>

#include <string.h>

Directory* root_dir = nullptr;

Directory& root_directory(){
	if(!root_dir)
		panic("calling root_directory() without initializing file system\n");
	return *root_dir;
}

DirectoryEntry::DirectoryEntry(const string& name, Inode* inode)
	: m_name(name), m_inode(inode){
}

bool DirectoryEntry::is_named(const string& name){
	return m_name == name;
}

size_t VFile::size(){
	return m_data.size();
}

size_t VFile::read(char* buffer, size_t offset, size_t amount){
	if(offset > m_data.size())
		return 0;

	size_t end = offset + amount;
	size_t ret = 0;
	for(size_t i = offset; i < end && i < size(); i++, ret++)
		buffer[ret] = m_data[i];

	return ret;
}

size_t VFile::write(char* buffer, size_t offset, size_t amount){
	ScopedLocker locker(&m_lock);

	if(offset > m_data.size())
		return 0;

	size_t end = offset + amount;
	size_t ret = 0;
	for(size_t i = offset; i < end; i++, ret++)
		m_data.insert(i, buffer[ret]);

	return ret;
}

size_t DeviceFile::size(){
	return m_data.size();
}

size_t DeviceFile::read(char* buffer, size_t offset, size_t amount){
	ScopedLocker locker(&m_lock);

	while(size() == 0)
		m_cvar.wait(m_lock);

	size_t ret = 0;
	for(; ret < amount && !m_data.is_empty(); ret++)
		buffer[ret] = m_data.remove_front();

	// If we didn't wake here any waiting threads would be starved till
	// the next write
	if(size() != 0)
		m_cvar.wake();

	return ret;
}

size_t DeviceFile::write(char* buffer, size_t offset, size_t amount){
	//we just ignore the offset

	size_t ret = 0;
	for(; ret < amount; ret++)
		m_data.insert_end(buffer[ret]);

	//wake up any waiting threads
	m_cvar.wake();

	return ret;
}

OutStream& operator<<(OutStream& stream, File& file){
	char buffer[21];
	for(size_t i = 0; i < file.size();){
		size_t read = file.read(buffer, i, 20);
		i+= read;
		buffer[read] = '\0';
		stream << buffer;
	}
	return stream;
}

Directory::Directory(){
	add_entry(".", this);
}

bool Directory::add_entry(const string& name, Inode* inode){
	m_entries.insert_end(DirectoryEntry(name, inode));
	return true;
}

DirectoryEntry* Directory::lookup_entry(const string& name){
	for(size_t i = 0; i < m_entries.size(); i++){
		DirectoryEntry* entry = &m_entries[i];
		if(entry->is_named(name))
			return entry;
	}
	return nullptr;
}

bool Directory::create_file(const string& name){
	VFile* file = new VFile();
	add_entry(name, file);
	return true;
}

bool Directory::create_subdirectory(const string& name){
	Directory* directory = new Directory();
	directory->add_entry("..", this);
	add_entry(name, directory);
	return true;
}

DirectoryEntry* Directory::lookup_path(const string& path){
	Vector<string> segments;

	string rem = path;
	for(int split = rem.index_of('/'); split != -1; split = rem.index_of('/')){
		string str = rem.substring(0, split);
		segments.insert_end(str);
		rem = rem.substring(split + 1);
	}
	segments.insert_end(rem);

	Directory* directory = this;
	DirectoryEntry* dir_ent = nullptr;

	for(int i = 0; i < segments.size() && directory != nullptr; i++){
		dir_ent = directory->lookup_entry(segments[i]);
		if(!dir_ent)
			return nullptr;
		directory = dir_ent->get_inode()->as_directory();
	}

	return dir_ent;
}

File* Directory::lookup_file(const string& path){
	DirectoryEntry* de = lookup_path(path);
	return de ? de->get_inode()->as_file() : nullptr;
}

Directory* Directory::lookup_directory(const string& path){
	DirectoryEntry* de = lookup_path(path);
	return de ? de->get_inode()->as_directory() : nullptr;
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
			VFile* file = new VFile();
			char* file_mem = (char*)init + (size_t)headers->file_pointer;
			file->write(file_mem, 0, headers->file_size);
			dir->add_entry(headers->name, file);
			headers++;
		}
		return true;
	}
	return false;
}

bool initialize_file_system(uint32_t* initfs){
	root_dir = new Directory();
	load_init_ramfs(root_dir, initfs);
}
