INCLUDE_FLAGS := -I.
CPPFLAGS := -ggdb3 $(INCLUDE_FLAGS) -m32 -ffreestanding -fno-stack-protector -Wall -nostdlib -fno-exceptions -fno-rtti
CC := g++

.PHONY: run clean all install
.SUFFIXES: .o .cpp .asm


OBJS =\
string.o \
Stream.o \

%.o : %.cpp
	$(CC) -c $(CPPFLAGS) $< -o $@

%.o : %.asm
	nasm -f elf32 $< -o $@

libk.a: $(OBJS)
	ar rcs $@ $(OBJS)

clean:
	rm -f *.a
	rm -f *.o
	rm -f data/*.o

