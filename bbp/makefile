AS = nasm -felf64 -O0
LD = x86_64-pc-elf-ld -melf_x86_64
OBJECTS = boot.o kernel.o

all: $(OBJECTS)
	$(LD) -T bbp.ld $(OBJECTS) -o ../Release/bbp.img

boot.o:
	make -C boot/

kernel.o:
	make -C kernel/	

%.s.o: %.asm
	$(AS) -o $@ $<

clean:
	rm -f *.o boot/*.o kernel/*.o ../Release/bbp.img ../Release/disk.img