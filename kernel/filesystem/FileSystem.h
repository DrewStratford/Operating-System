#pragma once

#include <data/Vector.h>
#include <devices/Serial.h>
#include <ConditionVar.h>

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
	File* as_file() { return this; };

	virtual size_t size() = 0;
	virtual size_t write(char*, size_t offset, size_t amount) = 0;
	virtual size_t read(char*, size_t offset, size_t amount) = 0;
};

class VFile : public File{
public:
	size_t size() override;
	size_t write(char*, size_t offset, size_t amount) override;
	size_t read(char*, size_t offset, size_t amount) override;

private:
	Vector<char> m_data;
};

class DeviceFile : public File{
public:
	size_t size() override;
	size_t write(char*, size_t offset, size_t amount) override;
	size_t read(char*, size_t offset, size_t amount) override;

private:
	Vector<char> m_data;
	ConditionVar m_cvar;
};

OutStream& operator<<(OutStream&, File&);

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
