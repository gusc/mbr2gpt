AS = nasm -felf -O0
#LD = x86_64-pc-elf-ld -melf_x86_64
#LD = x86_64-pc-elf-ld -melf_i386
LD = i786-elf-ld -i
LD64 = x86_64-pc-elf-ld -melf_x86_64
OC = x86_64-pc-elf-objcopy -I elf32-i386 -O elf64-x86-64
OBJECTS = boot16_64.o boot64.o

all: $(OBJECTS)
	$(LD64) -T bbp.ld $(OBJECTS) -o ../Release/bbp.img

boot16_64.o: boot16_32.o
	$(OC) boot16_32.o boot16_64.o

boot16_32.o: boot16.s.o boot32.o
	$(LD) -T boot16_32.ld boot16.s.o boot32.o -o boot16_32.o

boot32.o:
	make -C boot32/

boot64.o:
	make -C boot64/	

%.s.o: %.asm
	$(AS) -o $@ $<

clean:
	rm -f *.o boot32/*.o boot64/*.o ../Release/bbp.img ../Release/disk.img