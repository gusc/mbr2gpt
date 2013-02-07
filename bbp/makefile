AS = nasm -felf64 -O0
LD = x86_64-pc-elf-ld -melf_x86_64
OBJECTS = boot16.s.o boot32_64.o boot64.o

all: $(OBJECTS)
	$(LD) -T bbp.ld $(OBJECTS) -o ../Release/bbp.img

boot32_64.o:
	make -C boot32/

boot64.o:
	make -C boot64/	

%.s.o: %.asm
	$(AS) -o $@ $<

clean:
	rm -f *.o boot32/*.o boot64/*.o ../Release/bbp.img ../Release/disk.img