AS = nasm -felf
CC = i786-elf-gcc -march=i686 -nostdlib -fno-builtin -nostartfiles -nodefaultlibs
LD = i786-elf-ld

all: bbp.img

bbp.img: boot.o kmain.o descriptors.o interrupts.o
	$(LD) -T bbp.ld boot.o kmain.o descriptors.o interrupts.o -o ../Release/bbp.img

boot.o: boot.asm
	$(AS) -o boot.o boot.asm

kmain.o: kmain.c
	$(CC) -c kmain.c

descriptors.o: descriptors.asm
	$(AS) -o descriptors.o descriptors.asm

interrupts.o: interrupts.asm
	$(AS) -o interrupts.o interrupts.asm

clean:
	rm -f *.o ../Release/bbp.img ../Release/disk.img