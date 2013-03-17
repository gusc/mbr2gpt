AS = nasm -felf64 -O0
CC = x86_64-pc-elf-gcc -m32 -march=i686 -nostartfiles -nostdlib -nodefaultlibs -fno-builtin -Wno-attributes
LD = x86_64-pc-elf-ld -i
OC = x86_64-pc-elf-objcopy -I elf32-i386 -O elf64-x86-64
OBJECTS = boot.s.o bios.s.o main16.c64.o main32.c64.o

all: boot.o

boot.o: $(OBJECTS)
	$(LD) -T boot.ld $(OBJECTS) -o ../boot.o

%.s.o: %.asm
	$(AS) -o $@ $<

%.c.o: %.c
	$(CC) -c $< -o $@

%.c64.o: %.c.o
	$(OC) $< $@

clean:
	rm -f *.o