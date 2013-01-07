AS = nasm -felf
CC = i786-elf-gcc -march=i686 -nostdlib -fno-builtin -nostartfiles -nodefaultlibs
LD = i786-elf-ld

all: bbp.img

bbp.img: boot.o kmain.o main.o
	$(LD) -T bbp.ld boot.o kmain.o main.o -o ../Release/bbp.img

boot.o: boot.asm
	$(AS) -o boot.o boot.asm

main.o: main.asm
	$(AS) -o main.o main.asm

kmain.o: kmain.c
	$(CC) -c kmain.c

clean:
	rm -f boot.o main.o kmain.o ../Release/bbp.img ../Release/disk.img