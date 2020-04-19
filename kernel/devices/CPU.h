#pragma once

#include <stdint.h>

struct [[gnu::packed]] Registers{
	uint32_t ds;
	uint32_t es;
	uint32_t gs;
	uint32_t fs;
	
	//registers stored by pushad
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp0; 
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
	
	uint32_t intr;
	uint32_t err;
	
	uint32_t eip;
	uint32_t cs; 
	uint32_t flags;
	uint32_t esp;
	uint32_t ss;
};

class [[gnu::packed]] GDTEntry{
public:
	uint16_t limit_low { 0 };
	uint16_t base_low { 0 };
	uint8_t base_mid { 0 };
	uint8_t access { 0 };
	//limit is low flags is high
	uint8_t flags_limit_high { 0 };
	uint8_t base_high { 0 };

	/*
	void set_limit(uint32_t);
	void set_base(uint32_t);
	void set_flags(uint8_t);
	void set_access(uint8_t);
	*/
};

struct [[gnu::packed]] GDTPointer{
  uint16_t limit;
  GDTEntry *base;
};

struct [[gnu::packed]] TSS{
  uint16_t link;
  uint16_t r1;

  uint32_t esp0;
  uint16_t ss0;
  uint16_t r2;

  uint32_t esp1;
  uint16_t ss1;
  uint16_t r3;

  uint32_t esp2;
  uint16_t ss2;
  uint16_t r4;

  uint32_t cr3;
  uint32_t eip;
  uint32_t eflags;
  uint32_t eax;
  uint32_t ecx;
  uint32_t edx;
  uint32_t ebx;
  uint32_t esp;
  uint32_t ebp;
  uint32_t esi;
  uint32_t edi;

  uint32_t es;
  uint32_t cs;
  uint32_t ss;
  uint32_t ds;
  uint32_t fs;
  uint32_t gs;

  uint32_t ldtr;
  uint32_t IOPB;
};

void initialize_gdt_table();

class [[gnu::packed]] IDTEntry{
public:
	uint16_t offset_1;
	uint16_t code_sel;
	uint8_t zero;
	uint8_t type;
	uint16_t offset_2;

	IDTEntry();
	IDTEntry(uint32_t base, uint8_t type);
};

class [[gnu::packed]] IDTR{
public:
	uint16_t limit;
	uint32_t base;

	IDTR(uint16_t limit, uint32_t base);
};

extern "C" uint32_t get_eflags();
void sti();
void cli();

TSS& get_tss();

class NoInterrupts{
public:
	NoInterrupts();
	~NoInterrupts();
private:
	bool reenable { false };
};

void panic(char *);
