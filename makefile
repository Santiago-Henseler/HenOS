CFLAGS = -masm=intel -m32 -ffreestanding -nostdlib -nostdinc -fno-stack-protector -fno-pic -Wall -Wextra -g
LIBS = -Ilib -Ikernel/drivers  -Ikernel/memory -Ikernel/interrupts -Ikernel/syscall -Ikernel/fileSystem
KERNELFILES = kernel/*.c kernel/drivers/*/*.c kernel/drivers/*.c lib/*.c kernel/interrupts/*.c kernel/memory/*.c kernel/syscall/*.c kernel/fileSystem/*.c kernel/process/*.c

all: clean bootloader asm kernel.bin bootdisk run
debug: clean bootloader asm kernel.bin bootdisk gdb

clean: 
	rm kernel.bin kernel.elf bootloader *.o &>/dev/null
	clear

bootloader:
	nasm -f bin -I boot/ boot/bootloader.asm -o bootloader

asm:
	nasm -f elf32 kernel/kernelEntry.asm -o kernelEntry.o
	nasm -f elf32 kernel/drivers/io.asm -o ioAsm.o
	nasm -f elf32 kernel/interrupts/interrupts.asm -o asmInterrupts.o
	nasm -f elf32 kernel/memory/page.asm -o asmPage.o
	nasm -f elf32 kernel/syscall/syscall.asm -o asmSyscal.o
	nasm -f elf32 kernel/process/process.asm -o asmProcess.o

kernel.bin:	
	gcc $(CFLAGS) $(LIBS) -c $(KERNELFILES)
	ld -m elf_i386 -T link.ld *.o -o kernel.elf
	objcopy -O binary kernel.elf kernel.bin

bootdisk:
	dd if=/dev/zero of=disk.img bs=512 count=2880 # Comentar esta linea para mantener el filesystem
	dd conv=notrunc if=bootloader of=disk.img bs=512 count=1 seek=0
	dd if=kernel.bin of=disk.img bs=512 conv=notrunc seek=1
	dd if=a.o of=disk.img bs=512 conv=notrunc seek=52

run:
	qemu-system-i386 -machine q35 -m 512M -fda disk.img

gdb:
	qemu-system-i386 -machine q35 -fda disk.img -gdb tcp::26000 -S & \
	sleep 1; \
	gdb \
	  -ex "set architecture i386" \
	  -ex "file kernel.elf" \
	  -ex "target remote localhost:26000" \
	  -ex "layout split"