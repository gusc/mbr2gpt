AS = nasm -felf
CC = x86_64-pc-elf-gcc -m32 -march=i686 -nostdlib -fno-builtin -nostartfiles -nodefaultlibs
LD = x86_64-pc-elf-ld -melf_i386
OBJECTS = boot.s.o kmain.c.o descriptors.s.o interrupts.s.o screen.c.o memory.c.o string.c.o acpi.c.o

all: bbp.img

bbp.img: $(OBJECTS)
	$(LD) -T bbp.ld $(OBJECTS) -o ../Release/bbp.img

%.s.o: %.asm
	$(AS) -o $@ $<

%.c.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f *.o ../Release/bbp.img ../Release/disk.img