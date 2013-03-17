AS = nasm -felf64
CC = x86_64-pc-elf-gcc -nostdlib -fno-builtin -nostartfiles -nodefaultlibs
LD = x86_64-pc-elf-ld -i
OBJECTS = memory.c.o screen.c.o string.c.o kmain.c.o

all: kernel.o

kernel.o: $(OBJECTS)
	$(LD) -T kernel.ld $(OBJECTS) -o ../kernel.o

%.s.o: %.asm
	$(AS) -o $@ $<

%.c.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f *.o