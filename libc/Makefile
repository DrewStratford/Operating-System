INCLUDE_FLAGS := -I.
CPPFLAGS := -ggdb3 $(INCLUDE_FLAGS) -m32 -ffreestanding -fno-stack-protector -Wall -nostdlib -fno-exceptions -fno-rtti -fno-pie
CC := g++

.PHONY: run clean all install
.SUFFIXES: .o .cpp .asm


OBJS =\
string.o \
Stream.o \
FileStream.o \
Time.o \
icxxabi.o \
System.o \
Malloc.o \

KOBJS =$(OBJS:%.o=%.k.o)
UOBJS =$(OBJS:%.o=%.u.o)

%.u.o : %.cpp
	$(CC) -D USER_SPACE -c $(CPPFLAGS) $< -o $@

%.u.o : %.asm
	nasm -f elf32 $< -o $@

%.k.o : %.cpp
	$(CC) -D KERNEL_SPACE -c $(CPPFLAGS) $< -o $@

%.k.o : %.asm
	nasm -f elf32 $< -o $@

libk.a: $(KOBJS)
	ar rcs $@ $(KOBJS)

libc.a: $(UOBJS)
	ar rcs $@ $(UOBJS)

clean:
	rm -f *.a
	rm -f *.o
	rm -f data/*.o

