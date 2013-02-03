AS = nasm -felf
LD = x86_64-pc-elf-ld -melf_i386
#LD = i786-elf-ld
OBJECTS = boot16.s.o boot32/*.o
# boot64/*.o

all: boot16.s.o
	make -C boot32/
#	make -C boot64/
	$(LD) -T bbp.ld $(OBJECTS) -o ../Release/bbp.img

%.s.o: %.asm
	$(AS) -o $@ $<

clean:
	rm -f *.o boot32/*.o boot64/*.o ../Release/bbp.img ../Release/disk.img