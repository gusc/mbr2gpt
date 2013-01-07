AS = nasm.exe -felf
#i786-elf-as.exe
CC = i786-elf-gcc.exe -march=i686 -nostdlib -fno-builtin -nostartfiles -nodefaultlibs
LD = i786-elf-ld.exe

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
	rm -f main.o kmain.o ../Release/bbp.img