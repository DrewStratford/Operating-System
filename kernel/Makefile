INCLUDE_FLAGS := -I. -I../libc/
CPPFLAGS := -ggdb3 $(INCLUDE_FLAGS) -m32 -ffreestanding -fno-stack-protector -Wall -nostdlib -fno-exceptions -fno-rtti 
CC := g++

.PHONY: run clean all install
.SUFFIXES: .o .cpp .asm


OBJS =\
boot.o \
init.o \
icxxabi.o \
Thread.o \
Lock.o	\
devices/IO.o \
devices/Serial.o \
devices/CPU.o \
devices/Interrupts.o \
memory/Paging.o	\
memory/Region.o	\
memory/Heap.o	\
filesystem/FileSystem.o	\

all: os.iso

libc:
	make -C ../libc

%.o : %.cpp
	$(CC) -c $(CPPFLAGS) $< -o $@

%.o : %.asm
	nasm -f elf32 $< -o $@

os.kernel: $(OBJS)
	$(CC) -T linker.ld -o $@ $(CPPFLAGS) $(OBJS)

vfs: vfs/*
	../tools/makevfs vfs/*

os.iso: libc os.kernel vfs
	cp os.kernel ../bootdir/boot/
	cp myvfs.vfs ../bootdir/boot/init.initrd
	grub-mkrescue --directory=/usr/lib/grub/i386-pc -o $@ ../bootdir

run: os.iso
	qemu-system-i386 -serial stdio $<

debug: os.iso
	qemu-system-i386 -s -S -serial stdio $<

clean:
	rm -f $(OBJS)
	#rm -f *.o
	#rm -f devices/*.o
	rm -f os.kernel
	rm -f os.iso
	rm -f ../bootdir/boot/os.kernel
	make -C ../libc clean
