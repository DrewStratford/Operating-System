INCLUDE_FLAGS := -I.
CPPFLAGS := $(INCLUDE_FLAGS) -m32 -ffreestanding -fno-stack-protector -Wall -nostdlib -fno-exceptions -fno-rtti
CC := g++

.PHONY: run clean all install
.SUFFIXES: .o .cpp .asm


OBJS =\
boot.o \
init.o \
icxxabi.o \
devices/IO.o \
devices/Serial.o \
	
all: os.iso

%.o : %.cpp
	$(CC) -c $(CPPFLAGS) $< -o $@

%.o : %.asm
	nasm -f elf32 $< -o $@

os.kernel: $(OBJS)
	$(CC) -T linker.ld -o $@ $(CPPFLAGS) $(OBJS)

os.iso: os.kernel
	cp os.kernel ../bootdir/boot/
	grub-mkrescue --directory=/usr/lib/grub/i386-pc -o $@ ../bootdir

run: os.iso
	qemu-system-i386 -serial stdio $<

clean:
	rm -f *.o
	rm -f os.kernel
	rm -f os.iso
	rm ../bootdir/boot/os.kernel
