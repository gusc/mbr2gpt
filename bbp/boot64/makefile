AS = nasm -felf64
CC = x86_64-pc-elf-gcc -nostdlib -fno-builtin -nostartfiles -nodefaultlibs
LD = x86_64-pc-elf-ld 
OBJECTS = main64.c.o

all: boot64.o

boot64.o: $(OBJECTS)
	$(LD) -T boot64.ld $(OBJECTS) -o "../boot64.o"

%.s.o: %.asm
	$(AS) -o $@ $<

%.c.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f *.o