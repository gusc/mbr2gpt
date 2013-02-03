AS = nasm -felf
CC = x86_64-pc-elf-gcc -m32 -march=i686 -nostartfiles -nostdlib -nodefaultlibs -fno-builtin -Wno-attributes
#CC = i786-elf-gcc -march=i686 -nostartfiles -nostdlib -nodefaultlibs -fno-builtin -Wno-attributes
OBJECTS = gdt.s.o idt.s.o interrupts.s.o screen.c.o memory.c.o string.c.o acpi.c.o main32.c.o

all: $(OBJECTS)

%.s.o: %.asm
	$(AS) -o $@ $<

%.c.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f *.o