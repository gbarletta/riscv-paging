CC=riscv64-unknown-elf-gcc
CCFLAGS=-Wall -Wextra -std=c11 -pedantic -nostdlib -ffreestanding -mcmodel=medany -ftree-ter
AS=riscv64-unknown-elf-as
ASFLAGS=
LD=riscv64-unknown-elf-ld 
LDFLAGS=
EMU=qemu-system-riscv64
EMUFLAGS=-monitor stdio -machine virt -bios none

run: kernel.elf
	$(EMU) $(EMUFLAGS) -kernel kernel.elf

kernel.elf: link.ld boot.o kmain.o uart.o mm.o mem.o
	$(LD) $(LDFLAGS) -o kernel.elf -T link.ld boot.o kmain.o uart.o mm.o mem.o

boot.o: boot.s
	$(AS) boot.s -o boot.o $(ASFLAGS)

kmain.o: $(wildcard *.c) $(wildcard *.h)
	$(CC) -c uart.c -o uart.o $(CCFLAGS)
	$(CC) -c kmain.c -o kmain.o $(CCFLAGS)
	$(CC) -c mm.c -o mm.o $(CCFLAGS)
	$(CC) -c mem.c -o mem.o $(CCFLAGS)

clean:
	@rm -fr *.o *.elf
	
