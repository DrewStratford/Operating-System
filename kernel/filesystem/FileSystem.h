#pragma once

#include <data/Vector.h>
#include <devices/Serial.h>

class DirectoryEntry;
class Inode;
class File;
class Directory;


class Inode{
public:
	virtual File* as_file() { return nullptr; };
	virtual Directory* as_directory() { return nullptr; };
};

class DirectoryEntry{
public:
	DirectoryEntry(char*, Inode*);
	bool is_named(char*);

	Inode* get_inode() { return m_inode; };
	void set_inode(Inode* i) { m_inode = i; };

private:
	char* m_name;
	Inode* m_inode;
};


class File : public Inode{
public:
	virtual File* as_file() { return this;} ;

	size_t size();
	size_t write(char*, size_t offset, size_t amount);
	size_t read(char*, size_t offset, size_t amount);

private:
	Vector<char> m_data;

};

Serial& operator<<(Serial&, File&);

class Directory : public Inode{
public:
	Directory();
	virtual Directory* as_directory() { return this;} ;

	bool add_entry(char* name, Inode*);
	DirectoryEntry* lookup_entry(char* name);
	bool create_file(char* name);
	bool create_subdirectory(char* name);
	DirectoryEntry* lookup_path(char* path);
	Directory* lookup_directory(char* path);
	File* lookup_file(char* path);

private:
	Vector<DirectoryEntry> m_entries;

};

Directory& root_directory();
bool initialize_file_system(uint32_t* initfs);
