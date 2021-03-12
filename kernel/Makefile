INCLUDE_FLAGS := -I. -I../libc/
CPPFLAGS := -ggdb3 $(INCLUDE_FLAGS) -m32 -ffreestanding -fno-stack-protector -Wall -nostdlib -fno-exceptions -fno-rtti -Wno-write-strings -std=gnu++2a
CC := g++

.PHONY: run clean all install
.SUFFIXES: .o .cpp .asm


OBJS =\
boot.o \
init.o \
Thread.o \
Lock.o	\
ConditionVar.o	\
devices/IO.o \
devices/Serial.o \
devices/CPU.o \
devices/Interrupts.o \
devices/Keyboard.o \
devices/TTY.o \
devices/Clock.o \
memory/Paging.o	\
memory/PagingScope.o	\
memory/Region.o	\
filesystem/FileSystem.o	\

all: os.iso

libc:
	make -C ../libc libk.a
	make -C ../libc libc.a

userspace:
	make -C ../userspace
	../tools/makevfs ../userspace/*.prog


%.o : %.cpp
	$(CC) -c $(CPPFLAGS) $< -o $@

%.o : %.asm
	nasm -f elf32 $< -o $@

os.kernel: $(OBJS)
	$(CC) -T linker.ld -o $@ $(CPPFLAGS) $(OBJS)

os.iso: libc userspace os.kernel
	cp os.kernel ../bootdir/boot/
	cp myvfs.vfs ../bootdir/boot/init.initrd
	grub-mkrescue --directory=/usr/lib/grub/i386-pc -o $@ ../bootdir

run: os.iso
	qemu-system-i386 -enable-kvm -serial stdio -hdb hd.img $<

debug: os.iso
	qemu-system-i386 -s -S -serial stdio -hdb hd.img $<

clean:
	rm -f $(OBJS)
	rm -f os.kernel
	rm -f os.iso
	rm -f ../bootdir/boot/os.kernel
	make -C ../libc clean
	make -C ../userspace clean
