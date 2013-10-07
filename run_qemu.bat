ECHO Starting QEmu...

SET SDL_VIDEODRIVER=windib
SET QEMU_AUDIO_DRV=sdl
SET SDL_AUDIODRIVER=waveout
SET QEMU_AUDIO_LOG_TO_MONITOR=0

START qemu-system-x86_64w.exe -smp 2  -m 256M -L C:\Qemu\Bios -drive file=Release/disk.img,media=disk,if=none,id=mydisk -device ich9-ahci,id=ahci -device ide-drive,drive=mydisk,bus=ahci.0 -vga std -name MBR2GPT -rtc base=localtime,clock=host -no-reboot