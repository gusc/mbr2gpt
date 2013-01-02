AS = i786-elf-as.exe
CC = i786-elf-gcc.exe -march=i686 -nostdlib -fno-builtin -nostartfiles -nodefaultlibs
LD = i786-elf-ld.exe

all: bbp.img

bbp.img: main.o kmain.o
	$(LD) -T bbp.ld main.o kmain.o -o ../Release/bbp.img

main.o: main.asm
	$(AS) -o main.o main.asm

kmain.o: kmain.c
	$(CC) -c kmain.c

clean:
	rm -f main.o kmain.o ../Release/bbp.img