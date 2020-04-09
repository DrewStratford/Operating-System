#pragma once

#include <data/Vector.h>

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

class Directory : public Inode{
public:
	Directory();
	virtual Directory* as_directory() { return this;} ;

	bool add_entry(char* name, Inode*);
	DirectoryEntry* lookup_entry(char* name);
	bool create_file(char* name);
	bool create_subdirectory(char* name);

private:
	Vector<DirectoryEntry> m_entries;

};
