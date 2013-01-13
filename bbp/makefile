AS = nasm -felf
CC = i786-elf-gcc -march=i686 -nostdlib -fno-builtin -nostartfiles -nodefaultlibs
LD = i786-elf-ld
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