AS = nasm -fbin

all: mbr.img

mbr.img: main.asm
	$(AS) -o ../Release/mbr.img main.asm

clean:
	rm -f ../Release/mbr.img