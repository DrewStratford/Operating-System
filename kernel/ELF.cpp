#include <ELF.h>
#include <devices/Serial.h>


bool ELFHeader::is_elf(){
	return magic_number[0] == 0x7F
		&& magic_number[1] == 'E'
		&& magic_number[2] == 'L'
		&& magic_number[3] == 'F';
}

bool ELFHeader::is_elf_executable(){
	return is_elf() && type == 2;
}

ELFProgramHeader* ELFHeader::read_program_headers(File& file){
	auto* table = new ELFProgramHeader[program_header_count];
	auto rc = file.read((char*)table, header_table, program_header_count * sizeof(ELFProgramHeader));
	if( rc > 0 )
		return table;

	delete[] table;
	return nullptr;
}

ELFSectionHeader* ELFHeader::read_section_headers(File& file){
	auto* table = new ELFSectionHeader[section_header_count];
	auto rc = file.read((char*)table, section_table, section_header_count * sizeof(ELFSectionHeader));
	if( rc > 0 )
		return table;

	delete[] table;
	return nullptr;
}

char* ELFProgramHeader::type_as_string(){
}
