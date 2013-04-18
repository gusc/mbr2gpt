ECHO Starting QEmu...

SET SDL_VIDEODRIVER=windib
SET QEMU_AUDIO_DRV=sdl
SET SDL_AUDIODRIVER=waveout
SET QEMU_AUDIO_LOG_TO_MONITOR=0

START qemu-system-x86_64w.exe -smp 2  -m 256M -L C:\Qemu-windows-1.2.0\Bios -drive file=Release/disk.img,media=disk,if=virtio,cache=writeback -vga std -name MBR2GPT -rtc base=localtime,clock=host -no-reboot