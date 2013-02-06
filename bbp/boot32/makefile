AS = nasm -felf -O0
#CC = x86_64-pc-elf-gcc -m32 -march=i686 -nostartfiles -nostdlib -nodefaultlibs -fno-builtin -Wno-attributes
CC = i786-elf-gcc -march=i686 -nostartfiles -nostdlib -nodefaultlibs -fno-builtin -Wno-attributes
#LD = x86_64-pc-elf-ld -melf_i386 -i
LD = i786-elf-ld -i
OBJECTS = gdt.s.o idt.s.o interrupts.s.o screen.c.o memory.c.o string.c.o acpi.c.o main32.c.o

#all: $(OBJECTS)

all: boot32.o

boot32.o: $(OBJECTS)
	$(LD) -T boot32.ld $(OBJECTS) -o ../boot32.o

%.s.o: %.asm
	$(AS) -o $@ $<

%.c.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f *.o