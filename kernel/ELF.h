#pragma once
#include <cstdint>

#include <data/Vector.h>
#include <filesystem/FileSystem.h>


// Segment types:
// 0 = null
// 1 = load - clear memory, then load from file
// 2 = dynamic linking
// ...
class [[gnu::packed]] ELFProgramHeader{
public:
	uint32_t type;
	uint32_t p_offset; // Where the data is in the file
	uint32_t p_vaddr; // Where the data should be loaded in memory
	uint32_t undefined;
	uint32_t p_filesz; // size in file
	uint32_t p_memsz; // size in memory
	uint32_t flags; // executable = 1, writable = 2, 4 = readable
	uint32_t alignment;

	char* type_as_string();
};


class [[gnu::packed]] ELFSectionHeader{
public:
	uint32_t name; // pointer to the shstrtab
	uint32_t type;
	uint32_t flags;
	uint32_t sh_vaddr;
	uint32_t sh_offset;
	uint32_t link;
	uint32_t info;
	uint32_t alignment;
	uint32_t entry_size;
};

class [[gnu::packed]] ELFHeader{
public:
	char magic_number[4]; // 0x7F "ELF"
	uint8_t byte_size; // 1 = 32 bit, 2 = 64bit
	uint8_t endianess; // 1 = little endian, 2 = big endian
	uint8_t header_version;
	uint8_t os_abi; // Should be 0 for System V
	char padding[8];
	uint16_t type; // 1 = relocatable, 2 = executable, 3 = share, 4 = core
	uint16_t instruction_set;
	uint32_t elf_version;
	uint32_t entry_point;
	uint32_t header_table;
	uint32_t section_table;
	uint32_t flags;
	uint16_t header_size;
	uint16_t program_header_size;
	uint16_t program_header_count;
	uint16_t section_header_size;
	uint16_t section_header_count;
	uint16_t section_names_index;

	bool is_elf();
	bool is_elf_executable();
	ELFProgramHeader* read_program_headers(File& file);
	ELFSectionHeader* read_section_headers(File& file);
};
