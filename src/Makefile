CC:=gcc
CFLAGS:=-c -Wall -fno-stack-protector -m32
LD:=ld
LDFLAGS:=-T linker.ld -o kernel.sys -m elf_i386
ASM:=nasm
ASMFLAGS:=-f elf

all: kernel.o boot.o
	$(LD) $(LDFLAGS) kernel.o boot.o screen.o keyboard.o asmio.o asmisr.o gdt.o idt.o descriptors.o isr.o 

kernel.o: screen.o keyboard.o asmio.o descriptors.o
	$(CC) $(CFLAGS) kernel/kernel.c

boot.o: kernel.o
	$(ASM) $(ASMFLAGS) -o boot.o boot/boot.asm

gdt.o:
	$(ASM) $(ASMFLAGS) -o gdt.o kernel/gdt.asm

idt.o: isr.o
	$(ASM) $(ASMFLAGS) -o idt.o kernel/idt.asm

asmisr.o:
	$(ASM) $(ASMFLAGS) -o asmisr.o kernel/asmisr.asm

descriptors.o: asmisr.o gdt.o idt.o 
	$(CC) $(CFLAGS) kernel/descriptors.c

screen.o:
	$(CC) $(CFLAGS) kernel/screen.c
    
keyboard.o:
	$(CC) $(CFLAGS) kernel/keyboard.c
    
asmio.o:
	$(ASM) $(ASMFLAGS) -o asmio.o kernel/asmio.asm

isr.o:
	$(CC) $(CFLAGS) kernel/isr.c

clean:
	$(shell rm -rf *.o kernel.sys)
