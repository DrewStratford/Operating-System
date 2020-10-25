INCLUDE_FLAGS := -I. -I../libc/ -L../libc/
CPPFLAGS := -ggdb3 $(INCLUDE_FLAGS) -m32 -ffreestanding -fno-stack-protector -Wall -nostdlib -fno-exceptions -fno-rtti -fno-pie -Wno-write-strings
CC := g++

.PHONY: run clean all install
.SUFFIXES: .o .cpp .asm


OBJS =\
	crt0.o	\
	test.prog	\
	init.prog	\
	factorial.prog	\
	segfault.prog	\
	


all: $(OBJS)

libc:
	make -C ../libc libc.a

crt0.o : crt0.S
	$(CC) -c $(CPPFLAGS) $< -o $@

%.prog : %.cpp
	$(CC) -T linker.ld $(CPPFLAGS) $< -o $@

clean:
	rm -f $(OBJS)

